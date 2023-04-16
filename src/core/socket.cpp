#include "core/socket.h"

#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <cassert>

namespace pine {


constexpr static int BACK_LOG = 128;

Socket::Socket(int fd) noexcept
  : socket_fd_(fd) {}

Socket::Socket(Socket && other) noexcept {
  socket_fd_ = other.socket_fd_;
  other.socket_fd_ = -1;
}

auto Socket::operator = (Socket && other) noexcept -> Socket & {
  if (socket_fd_ != -1) {
    close(socket_fd_);
  }
  std::swap(socket_fd_, other.socket_fd_);
  return *this;
}

Socket::~Socket() {
  if (socket_fd_ != -1) {
    close(socket_fd_);
    socket_fd_ = -1;
  }
}

void Socket::Connect(const NetAddress & server_address) {
  if (socket_fd_ == -1) {
    CreateByProtocol(server_address.GetProtocol());
  }
  if ((connect(socket_fd_, server_address.YieldAddr(), *server_address.YieldAddrLen())) == -1) {
    // LOG_ERROR
    throw std::logic_error("Socket: connect() error");
  }
}

void Socket::Bind(const NetAddress & server_address, bool set_reuseable) {
  if (socket_fd_ == -1) {
    CreateByProtocol(server_address.GetProtocol());
  }
  
  if (set_reuseable) {
    SetReuseable();
  }

  if ((bind(socket_fd_, server_address.YieldAddr(), *server_address.YieldAddrLen())) == -1) {
    // TODO
    // LOG_ERROR
    throw std::logic_error("Socket: bind() error");
  }
}

void Socket::Listen() {
  assert(socket_fd_ != -1 && "Can't listen with an invalid fd");
  if ((listen(socket_fd_, BACK_LOG)) == -1) {
    // TODO
    throw std::logic_error("Socket: listen() error");
  }
}

auto Socket::Accept(const NetAddress & client_address) -> int{
  assert(socket_fd_ != -1);
  int client_fd = accept(socket_fd_, client_address.YieldAddr(), client_address.YieldAddrLen());
  if (client_fd == -1) {
    // Under high pressure, accept might fail.
    // LOG_WARNING
  }
  return client_fd;
}

void Socket::SetNonBlocking() {
  assert(socket_fd_ != -1);
  if ((fcntl(socket_fd_, F_SETFL, GetAttrs() | O_NONBLOCK)) == -1) {
    // 
    throw std::logic_error("Socket: set nonblocking error");
  }
}


void Socket::SetReuseable() {
  assert(socket_fd_ != -1);
  int yes = 1;

  if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 || 
      setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
    //
    throw std::logic_error("Socket: setsockopt() error");
  }
}

auto Socket::GetAttrs() -> int {
  assert(socket_fd_ != -1);
  return fcntl(socket_fd_, F_GETFL);
}

}