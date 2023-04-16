#include "core/thread_pool.h"

namespace pine {

ThreadPool::ThreadPool(int size) {
  size = std::max(size, MIN_NUM_SIZE);
  for (int i =0 ; i < size; ++i) {
    threads_.emplace_back([this]() {
      while (true) {
        std::function<void()> next_task;
        {
          std::unique_lock<std::mutex> lk{mutex_};
          cv_.wait(lk, [this]{  // 当退出或者任务队列不为空时唤醒
            return exit_ || !tasks_.empty();
          });
          if (exit_ && tasks_.empty()) { // thread ends life
            break;
          }
          next_task = tasks_.front();
          tasks_.pop();
        }
        next_task(); // execute
      }
    });
  }
}

void ThreadPool::Exit() {
  exit_ = true;
  cv_.notify_all();
}

ThreadPool::~ThreadPool() {
  Exit();
  for (auto &t : threads_) {
    if (t.joinable()) {
      t.join();
    }
  }
}

}