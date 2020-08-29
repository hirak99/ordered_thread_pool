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
#ifndef ORDERED_THRED_POOL_H
#define ORDERED_THRED_POOL_H

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
    std::lock_guard<std::mutex> lck(fn_queue_mtx_);
    fn_queue_.push(Job{.job_fn = fn,
                       .completion_fn = on_completion,
                       .ticket_no = job_count_++});
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
    JobFnT job_fn;
    CompletionFnT completion_fn;
    size_t ticket_no;
  };

  // Blocks till a next job is available, or termination signal is received.
  // If termination is requested, returns empty.
  std::optional<Job> NextJob() {
    std::unique_lock<std::mutex> lck(fn_queue_mtx_);
    fprintf(stderr, "Nextjob...\n");
    new_job_.wait(lck, [this] { return !fn_queue_.empty() || terminate_now_; });
    // If requested to terminate, finish the entire queue and exit.
    if (terminate_now_ && fn_queue_.empty()) {
      return {};
    }
    Job result = fn_queue_.front();
    fn_queue_.pop();
    fprintf(stderr, "Returning job %ld\n", result.ticket_no);
    return result;
  }

  void Worker() {
    fprintf(stderr, "Worker\n");
    while (true) {
      fprintf(stderr, "Looking for job...\n");
      std::optional<Job> job_opt = NextJob();
      if (!job_opt.has_value()) {
        // This means the workers should terminate.
        return;
      }
      Job job = job_opt.value();
      ReturnType result = job.job_fn();

      // Wait till our turn comes.
      std::unique_lock<std::mutex> lck(ticket_mtx_);
      ticket_update_.wait(
          lck, [this, &job] { return ticket_num_ == job.ticket_no; });
      // Perform the second part of the task.
      job.completion_fn(result);
      // Update the next ticket and send a signal to other workers in line.
      ++ticket_num_;
      ticket_update_.notify_all();
    }
    fprintf(stderr, "Worker ends\n");
  }

  // The worker threads are initialized on construction and maintained.
  std::vector<std::thread> workers_;
  // Queue of functions to execute.
  std::queue<Job> fn_queue_;
  std::mutex fn_queue_mtx_;
  std::condition_variable new_job_;
  size_t job_count_ = 0;

  // Ticket system to ensure chronological delivery of jobs. The next job
  // matching this is allowed to call the completion_fn.
  size_t ticket_num_ = 0;
  // The mutex to lock for second function.
  std::mutex ticket_mtx_;
  std::condition_variable ticket_update_;

  // If true, the worker threads should stop.
  bool terminate_now_ = false;
};

#endif  // ORDERED_THRED_POOL_H
