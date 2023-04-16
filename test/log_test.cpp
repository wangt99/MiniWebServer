#include "common/logger.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

static void PrintLog() {
  LOG_INFO("Hello world!");
  LOG_WRANING("Hello world!");
  LOG_ERROR("Hello world!");
}

static void StressLog() {
  for (int i = 0; i < 500; i++) {
    std::string s = std::to_string(i) + " Hello world!";
    LOG_INFO(s);
    LOG_WRANING(s);
    LOG_ERROR(s);
  }
}

TEST_CASE("TestLog", "PrintLog") {
  // PrintLog();
  StressLog();
}