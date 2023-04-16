/**
 * @file poller.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "common/logger.h"
#include "core/connection.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <sys/types.h>
#include <vector>


namespace pine {

constexpr uint32_t DEFAULT_POLL_SIZE = 1024;
constexpr uint32_t POLL_ADD = EPOLL_CTL_ADD;
constexpr uint32_t POLL_ET = EPOLLET;
constexpr uint32_t POLL_READ = EPOLLIN;

/**
 * @brief The poller act at socket monitor that activately polling on connections.
 * 
 */
class Poller {
using PollEvent = struct epoll_event;

public:
  explicit Poller(int poll_size = DEFAULT_POLL_SIZE)
    : poll_size_(poll_size) {
      poll_fd_ = epoll_create1(0);
      if (poll_fd_ == -1) {
        LOG_ERROR("Poller: epoll_creat1 error");
        exit(EXIT_FAILURE);
      }
      poll_envents = new PollEvent[poll_size_];
      memset(poll_envents, 0, poll_size_ * sizeof(PollEvent));
  }

  ~Poller() {
    if (poll_fd_ != -1) {
      close(poll_fd_);
      delete[] poll_envents;
      poll_fd_ = -1;
    }
  }

  auto GetPollSize() const -> int {
    return poll_size_;
  }

  void AddConnection(Connection *conn);

  auto Poll(int timeout = -1) -> std::vector<Connection *>;

private:
  int poll_fd_ {-1};
  int poll_size_;
  PollEvent *poll_envents {nullptr};
};


}
