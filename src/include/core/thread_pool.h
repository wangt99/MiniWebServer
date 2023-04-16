#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <queue>
#include <algorithm>
#include <future>

namespace pine {

constexpr int MIN_NUM_SIZE = 2;

class ThreadPool {
public:
  explicit ThreadPool(int size);

  ~ThreadPool();

  auto Size() const -> int { return threads_.size(); }

  void Exit();

  template<typename F, typename... Args>
  auto SubmitTask(F &&new_task, Args&& ...args) -> decltype(auto);

private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_; 

  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> exit_ {false};
};

template<typename F, typename... Args>
auto ThreadPool::SubmitTask(F &&new_task, Args&&... args) -> decltype(auto) {
  // 理论上，任务队列可以无限大
  using return_type = std::invoke_result_t<F, Args...>;
  if (exit_) {
    std::runtime_error("ThreadPool: SubmitTask() called while already exit_ being true");
  }
  // 为了实现异步调用，首先得对task包装
  auto packaged_new_task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<F>(new_task), std::forward<Args>(args)...));

  auto fut = packaged_new_task->get_future();
  {
    std::unique_lock<std::mutex> lk{mutex_};
    // 放进任务队列
    tasks_.emplace([packaged_new_task](){
      (*packaged_new_task)();
    });
  }
  cv_.notify_one();
  return fut;
}

}