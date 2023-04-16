#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <netdb.h>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>

namespace pine {


enum class Protocol { IPV4, IPV6, };

class NetAddress {

using IPv4Addr = struct sockaddr_in;
using IPv6Addr = struct sockaddr_in6;

public:

  NetAddress() noexcept;
  NetAddress(const char * ip, uint16_t port, Protocol protocol = Protocol::IPV4) noexcept;
  NetAddress(const std::string & ip, uint16_t port, Protocol protocol = Protocol::IPV4) noexcept
    : NetAddress(ip.data(), port, protocol) {}

  auto GetIp() const noexcept -> std::string;
  auto GetPort() const noexcept -> uint16_t;
  auto GetProtocol() const -> Protocol;

  auto YieldAddr() const -> struct sockaddr *;
  auto YieldAddrLen()  const -> socklen_t *;

  auto ToString() const -> std::string; 

private:
  // we set IPv4 as the default protocol
  Protocol protocol_{Protocol::IPV4};
  mutable struct sockaddr_storage addr_ {};
  mutable socklen_t addr_len_;
};

auto operator<<(std::ostream &os, const NetAddress & net_addr) -> std::ostream &;

}