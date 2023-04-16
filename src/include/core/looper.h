#pragma once

#include "core/connection.h"
#include "core/poller.h"
#include <atomic>
#include <thread>
#include <memory>
#include <map>
#include <mutex>

namespace pine {

/* the epoll_wait time in milliseconds*/
constexpr int TIME_OUT = 3000;

class Looper {
public:
  Looper() : poller_{std::make_unique<Poller>()} {}

  ~Looper() = default;

  void Loop();
  
  /* Add a new connection into looper */
  void AddConnection(std::unique_ptr<Connection> new_conn);

  /* if the connection exist in looper, Remove it from looper, otherwise, return false */
  auto RemoveConnection(int fd) -> bool;

  /* Add a accpetor connection into looper */
  void AddAcceptor(Connection *acceptor_conn);

  void SetExit() noexcept {
    std::unique_lock<std::mutex> lk{mutex_};
    is_exit_ = true;
  }

private:
  std::unique_ptr<Poller> poller_;
  std::map<int, std::shared_ptr<Connection>> connections_;
  bool is_exit_{false};
  std::mutex mutex_;
};

}
