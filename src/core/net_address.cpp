#include "core/net_address.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <type_traits>

namespace pine {

  NetAddress::NetAddress() noexcept {
    memset(&addr_, 0, sizeof(addr_));
    addr_len_ = sizeof(addr_);
  }

  NetAddress::NetAddress(const char * ip, uint16_t port, Protocol protocol) noexcept
    : protocol_(protocol) {
      memset(&addr_, 0, sizeof(addr_));
      addr_len_ = sizeof(addr_);
      if (protocol == Protocol::IPV4) {
        auto ipv4_addr = reinterpret_cast<struct sockaddr_in *>(&addr_);
        ipv4_addr->sin_family = AF_INET;
        inet_pton(AF_INET, ip, &ipv4_addr->sin_addr);
        ipv4_addr->sin_port = htons(port);
      } else {
        auto ipv6_addr = reinterpret_cast<struct sockaddr_in6*>(&addr_);
        ipv6_addr->sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip, &ipv6_addr->sin6_addr);
        ipv6_addr->sin6_port = htons(port);
      }
    }

  /* Access the net address information functions. */

  auto NetAddress::GetProtocol() const -> Protocol {
    return protocol_;
  }

  auto NetAddress::GetIp() const noexcept -> std::string {
    char ip_addr[INET6_ADDRSTRLEN];
    if (protocol_ == Protocol::IPV4) {
      auto ipv4_addr = reinterpret_cast<const IPv4Addr *>(&addr_);
      inet_ntop(AF_INET, &ipv4_addr->sin_addr, ip_addr, INET_ADDRSTRLEN);
    } else {
      auto ipv6_addr = reinterpret_cast<const IPv6Addr *>(&addr_);
      inet_ntop(AF_INET6, &ipv6_addr->sin6_addr, ip_addr, INET6_ADDRSTRLEN);
    }
    return ip_addr;
  }

  auto NetAddress::GetPort() const noexcept -> uint16_t {
    uint16_t port;
    if (protocol_ == Protocol::IPV4) {
      auto ipv4_addr = reinterpret_cast<const IPv4Addr *>(&addr_);
      port = ntohs(ipv4_addr->sin_port);
    } else {
      auto ipv6_addr = reinterpret_cast<const IPv6Addr *>(&addr_);
      port = ntohs(ipv6_addr->sin6_port);
    }
    return port;
  }

  auto NetAddress::YieldAddr() const -> struct sockaddr * {
    return reinterpret_cast<struct sockaddr *>(&addr_);
  }

  auto NetAddress::YieldAddrLen() const -> socklen_t * {
    return &addr_len_;
  }

  auto NetAddress::ToString() const -> std::string {
    return GetIp() + "@" + std::to_string(GetPort());
  }


  auto operator << (std::ostream & os, const NetAddress & net_addr) -> std::ostream & {
    os << net_addr.ToString() << '\n';
    return os;
  }

}