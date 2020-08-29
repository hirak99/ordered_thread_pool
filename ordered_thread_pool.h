// Implements a synchronized thread pool.
//
// This parallelizes jobs of type -
//
//   while (...) {
//     quick_fn(heavy_fn());
//   }
//
// To parallelize, use the following pattern -
//
//   pool = OrderedThreadPool{10};
//   while (...) {
//     pool.Do(heavy_fn, quick_fn);
//   }
//
// Properties -
// - All heavy_fn()'s are allowed to run in parallel.
// - All quick_fn()'s are called maintaining order of queuing.
// - Avoids repeated thread creation by reusing threads.
// - If workers are full, this blocks untill next one is free.
// - On destruction blocks untill all pending work is finished.
//
#ifndef ORDERED_THREAD_POOL_H
#define ORDERED_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

template <class ReturnType>
class OrderedThreadPool {
  using JobFnT = std::function<ReturnType()>;
  using CompletionFnT = std::function<void(ReturnType)>;

 public:
  // If num_workers is 0, multi-threading will be disabled and Do() will block
  // the main thread.
  OrderedThreadPool(int num_workers) {
    for (int i = 0; i < num_workers; ++i) {
      workers_.push_back(std::thread(&OrderedThreadPool::Worker, this));
    }
  }

  // Movable but not copyable.
  OrderedThreadPool(OrderedThreadPool&& other);
  OrderedThreadPool& operator=(OrderedThreadPool&& other);

  // The main public method for queuing a job. This will block if all threads
  // are busy until any one is freed.
  // The on_completion method will be called in the order the jobs were started.
  void Do(JobFnT fn, CompletionFnT on_completion) {
    if (workers_.empty()) {
      // Number of threads requested is 0. Run everything on main thread.
      on_completion(fn());
      return;
    }
    // Push to the job queue and notify.
    std::lock_guard<std::mutex> lck(fn_queue_mtx_);
    fn_queue_.push(Job{
        .job_fn = fn, .completion_fn = on_completion, .job_id = job_count_++});
    new_job_.notify_one();
  }

  virtual ~OrderedThreadPool() {
    terminate_now_ = true;
    new_job_.notify_all();
    for (std::thread& t : workers_) {
      t.join();
    }
  }

 private:
  struct Job {
    // A function which will be parallelized.
    JobFnT job_fn;
    // A quick function. Output of job_fn will be passed to this method. All
    // calls to this will happen in same order as enqueuing, and under a lock.
    CompletionFnT completion_fn;
    // Internal ticket number. Used in waiting for previous jobs.
    size_t job_id;
  };

  // Blocks till a next job is available, or termination signal is received.
  // If termination is requested, returns empty.
  std::optional<Job> NextJob() {
    std::unique_lock<std::mutex> lck(fn_queue_mtx_);
    new_job_.wait(lck, [this] { return !fn_queue_.empty() || terminate_now_; });
    // If requested to terminate, finish the entire queue and exit.
    if (terminate_now_ && fn_queue_.empty()) {
      return {};
    }
    Job result = fn_queue_.front();
    fn_queue_.pop();
    return result;
  }

  void Worker() {
    while (true) {
      std::optional<Job> job_opt = NextJob();
      if (!job_opt.has_value()) {
        // This means the workers should terminate.
        return;
      }
      Job job = job_opt.value();

      // This runs parallelly across all threads.
      ReturnType result = job.job_fn();

      // Wait till our turn comes.
      std::unique_lock<std::mutex> lck(ticket_mtx_);
      ticket_update_.wait(lck,
                          [this, &job] { return ticket_num_ == job.job_id; });
      // Perform the second part of the task.
      job.completion_fn(result);
      // Update the next ticket and send a signal to other workers in line.
      ++ticket_num_;
      ticket_update_.notify_all();
    }
  }

  // The worker threads are initialized on construction and maintained.
  std::vector<std::thread> workers_;
  // Queue of functions to execute.
  std::queue<Job> fn_queue_;
  std::mutex fn_queue_mtx_;
  std::condition_variable new_job_;
  // Incremental job_id passed to each job.
  size_t job_count_ = 0;

  // Ticket system to ensure chronological delivery of jobs. The next job
  // with job_id matching this will proceed with completion_fn().
  size_t ticket_num_ = 0;
  // The mutex to lock for second function.
  std::mutex ticket_mtx_;
  std::condition_variable ticket_update_;

  // If true, the worker threads should stop.
  bool terminate_now_ = false;
};

#endif  // ORDERED_THREAD_POOL_H
