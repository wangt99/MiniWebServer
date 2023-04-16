#include "core/poller.h"
#include "common/logger.h"
#include "core/connection.h"
#include <cassert>
#include <cstdlib>
#include <sys/epoll.h>

namespace pine {

// constexpr int 

/**
 * @brief add connection into epoll events
 * 
 * @param conn 
 */
void Poller::AddConnection(Connection * conn) {
  assert(conn->GetFd() != -1);
  PollEvent event;
  event.data.ptr = conn;
  event.events = conn->GetEvents();
  
  auto ret = epoll_ctl(poll_fd_, POLL_ADD, conn->GetFd(), &event);
  if (ret == -1) {
    LOG_ERROR("Poller::AddConnection epoll_ctl error");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief poll all ready connection
 * 
 * @param timeout 
 * @return std::vector<Connection * > 
 */
auto Poller::Poll(int timeout) -> std::vector<Connection * > {
  std::vector<Connection *> events_happen;
  int ready_num = epoll_wait(poll_fd_, poll_envents, poll_size_, timeout);
  if (ready_num == -1) {
    // LOG_ERROR("Poller: epoll_wait error");
    std::cout << "Poller: epoll_wait error\n";
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < ready_num; ++i) {
    auto ready_conn = reinterpret_cast<Connection *>(poll_envents[i].data.ptr);
    ready_conn->SetRevents(poll_envents[i].events);
    events_happen.push_back(ready_conn);
  }
  return events_happen;
}
}