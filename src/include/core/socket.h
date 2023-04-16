#pragma once

#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include "core/net_address.h"

namespace pine {

class NetAddress;
enum class Protocol;

/**
 * @brief Socket Wrapper
 * This class encapsulates a socket descriptor, which is used for client or listener. 
 */

class Socket {
public:
  Socket() = default;

  explicit Socket(int fd) noexcept;

  Socket(Socket &&) noexcept;

  auto operator = (Socket &&) noexcept -> Socket &;

  ~Socket();

  auto GetFd()  -> int {
    return socket_fd_;
  }

  /// for client 
  void Connect(const NetAddress & server_address);

  /// for server
  void Bind(const NetAddress & server_address, bool set_reuseable = true);

  void Listen();

  auto Accept(const NetAddress & client_address) -> int;

  /* other tool functions */
  void SetReuseable();

  void SetNonBlocking();

  auto GetAttrs() -> int;

private:
  int socket_fd_ {-1};

  void CreateByProtocol(Protocol protocol) {
    if (protocol == Protocol::IPV4) {
      socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    } else {
      socket_fd_ = socket(AF_INET6, SOCK_STREAM, 0);
    }
    
    if (socket_fd_ == -1) {
      // TODO
      // throw exception and print log
      throw std::logic_error("Socket: socket() error.");
    }
  }
};

}