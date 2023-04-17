#include "core/looper.h"
#include "core/connection.h"
#include <cstdlib>
#include <memory>
#include <mutex>

namespace pine {

void Looper::Loop() {
  // std::cout << "enter fucking loop...\n";
  while(!is_exit_) {
    // ...
    // std::cout << "fucking loop....\n";
    auto ready_conns = poller_->Poll(TIME_OUT);
    // std::cout << "ready connections nums = " << ready_conns.size() << '\n';
    for (auto conn : ready_conns) {
      conn->GetCallback()();
    }
    // std::cout << " callback has been executed.\n";
  }
  // std::cout << "exit fucking loop...\n";
}

void Looper::AddConnection(std::unique_ptr<Connection> new_conn) {
  std::unique_lock<std::mutex> lk{mutex_};
  poller_->AddConnection(new_conn.get());
  int fd = new_conn->GetFd();
  connections_.insert({fd, std::move(new_conn)});
}

void Looper::AddAcceptor(Connection *acceptor_conn) {
  std::unique_lock<std::mutex> lk{mutex_};
  poller_->AddConnection(acceptor_conn);
}

auto Looper::RemoveConnection(int fd) -> bool {
  std::unique_lock<std::mutex> lk{mutex_};
  if (connections_.find(fd) != connections_.end()) {
    connections_.erase(fd);
    return true;
  }
  return false;
}

}