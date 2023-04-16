#include "core/buffer.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>


TEST_CASE("Buffer Test", "[core/buffer]") {
  using pine::Buffer;
  using pine::INTIAL_BUFFER_SIZE;

  Buffer buffer(INTIAL_BUFFER_SIZE);

  REQUIRE(buffer.Size() == 0);
  REQUIRE(buffer.Capacity() == INTIAL_BUFFER_SIZE);

  SECTION("appending c-style str into buffer") {
    char a[128] = {"Hello World!"};
    char b[512] = {"Bye! Good night."};
    buffer.AppendHead(a, strlen(a));
    buffer.Append(b, strlen(b)); 
    sprintf(a + strlen(a), "%s", b);
    CHECK(buffer.Size() == strlen(a));
    CHECK(strncmp(buffer.Data(), a, strlen(a)) == 0);
    buffer.Clear();
    CHECK(buffer.Size() == 0);
  }

  SECTION("appending std::string into buffer from both side") {
    const std::string msg1 = "Greeting from beginning!";
    const std::string msg2 = "Farewell from end~";
    buffer.AppendHead(msg1);
    buffer.Append(msg2);
    const std::string together = msg1 + msg2;
    CHECK(buffer.ToStringView() == together);
    buffer.Clear();
    CHECK(buffer.Size() == 0);
  }

  SECTION("find and pop based on the target first found") {
    const std::string msg =
        "GET / HTTP/1.1\r\n"
        "Connection: Keep-Alive\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Accept-Language: en-us\r\n"
        "\r\n";
    const std::string next_msg = "Something belongs to next message";
    buffer.Append(msg);
    buffer.Append(next_msg);
    auto op_str = buffer.FindAndPopTill("\r\n\r\n");
    CHECK((op_str.has_value() && op_str.value() == msg));
    CHECK(buffer.ToStringView() == next_msg);
  }
}