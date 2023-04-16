#include "core/net_address.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Net Address Test", "[core/net_address]") {
  using pine::NetAddress;
  using pine::Protocol;

  SECTION("IPv4 addresss case") {
    NetAddress ipv4("127.0.0.1", 1010, Protocol::IPV4);
    REQUIRE(ipv4.GetProtocol() == Protocol::IPV4);
    REQUIRE(ipv4.GetIp() == "127.0.0.1");
    REQUIRE(ipv4.GetPort() == 1010);
    REQUIRE(ipv4.ToString() == std::string("127.0.0.1@1010"));
  }

  SECTION("IPv6 address case1") {
    NetAddress ipv6("::1", 1011, Protocol::IPV6);
    REQUIRE(ipv6.GetProtocol() == Protocol::IPV6);
    REQUIRE(ipv6.GetIp() == "::1");
    REQUIRE(ipv6.GetPort() == 1011);
  }

  SECTION("IPv6 address case2") {
    NetAddress ipv6("fe80::6e7a:b6e0:98a9:5b4e", 1011, Protocol::IPV6);
    REQUIRE(ipv6.GetProtocol() == Protocol::IPV6);
    REQUIRE(ipv6.GetIp() == "fe80::6e7a:b6e0:98a9:5b4e");
    REQUIRE(ipv6.GetPort() == 1011); 
    REQUIRE(ipv6.ToString() == std::string("fe80::6e7a:b6e0:98a9:5b4e@1011"));
  }
}

