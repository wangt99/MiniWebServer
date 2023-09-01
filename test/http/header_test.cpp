#include "http/header.h"
#include "http/http_utils.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>

using namespace pine::http;

TEST_CASE("Header Test", "[http/header]") {
  SECTION("normal constructor method for header") {
    std::string k = "http_version";
    std::string v1 = "1.1";
    std::string v2 = "1.2";
    Header h1(k, v1);
    CHECK(h1.IsValid());
    CHECK(h1.Key() == k);
    CHECK(h1.Value() == v1);
    h1.SetValue(v2);
    CHECK(h1.Value() == v2);
  }

  SECTION("serialize/desericalize a header from string") {
    std::string valid_header_line = "Name:Tom";
    std::string invalid_header_line = "Hello & World";
    std::string request_line = "GET /index.html";
    Header h1(valid_header_line);
    Header h2(invalid_header_line);
    Header h3(request_line);

    CHECK(h1.IsValid());
    CHECK(h1.Key() == "Name");
    CHECK(h1.Value() == "Tom");

    CHECK(!h2.IsValid());
    CHECK(!h3.IsValid());
    
    std::string serialized = h1.Serialize();
    CHECK(serialized == (valid_header_line + CRLF));
  }
}