#pragma once

#include "core/acceptor.h"
#include "core/connection.h"
#include "core/looper.h"
#include "core/poller.h"
#include "core/buffer.h"
#include "core/thread_pool.h"
#include "core/net_address.h"
#include "core/socket.h"

#include <iterator>
#include <stdexcept>
#include <thread>
#include <vector>
#include <memory>
#include <algorithm>


namespace pine {

class PineServer {
public:
  PineServer(const NetAddress &server_addr, int concurrency = static_cast<int>(std::thread::hardware_concurrency() - 1))
    : pool_(std::make_unique<ThreadPool>(concurrency)), listener_(std::make_unique<Looper>()) {
      // build reactors      
      for (int i = 0; i < pool_->Size(); ++i) {
        reactors_.push_back(std::make_unique<Looper>());
      }
      // for acceptor
      std::vector<Looper *> raw_reactors;
      std::transform(reactors_.begin(), reactors_.end(), std::back_inserter(raw_reactors),
        [](auto &uni_ptr) { return uni_ptr.get(); });
      // setup pool
      for (auto &r : reactors_) {
        pool_->SubmitTask([&]{ r.get()->Loop(); });
      }
      // build acceptor
      accpetor_ = std::make_unique<Acceptor>(listener_.get(), raw_reactors, server_addr);
    }

  virtual ~PineServer() = default;

  auto OnAccept(std::function<void(Connection *)> on_accept) -> PineServer & {
    accpetor_->SetCustomAcceptCallback(on_accept);
    return *this;
  }

  auto OnHnadle(std::function<void(Connection *)> on_handle) -> PineServer &  {
    accpetor_->SetCustomHandleCallback(on_handle);
    on_handle_set = true;
    return *this;
  }

  void Run() {
    if (!on_handle_set) {
      std::logic_error("Please specify OnHandle callback function before run");
    }
    listener_->Loop();
  }

private:
  std::unique_ptr<ThreadPool> pool_;
  std::unique_ptr<Looper> listener_;
  std::unique_ptr<Acceptor> accpetor_;
  std::vector<std::unique_ptr<Looper>> reactors_;
  bool on_handle_set {false};
};

}