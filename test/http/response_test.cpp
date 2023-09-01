/**
 * @file response_test.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "http/response.h"

#include "catch2/catch_test_macros.hpp"
#include "http/header.h"
#include "http/http_utils.h"

using namespace pine::http;

TEST_CASE("[http/response]") {
  SECTION("response should be able to modify header on the fly") {
    std::string status = "200 Success";
    Response response{RESPONSE_OK, false, std::string("nonexistent-file.txt")};
    auto headers = response.GetHeaders();
    bool find = false;
    for (auto &h : headers) {
      if (h.Key() == HEADER_CONTENT_LENGTH) {
        find = true;
      }
    }
    CHECK(find);
    std::string new_val = "1024";
    CHECK(response.ChangeHeader(HEADER_CONTENT_LENGTH, new_val));
    find = false;
    std::string value;
    headers = response.GetHeaders();
    for (auto &h : headers) {
      if (h.Key() == HEADER_CONTENT_LENGTH) {
        find = true;
        value = h.Value();
      }
    }
    CHECK(find);
    CHECK(value == new_val);
  }
}
