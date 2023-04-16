#include <catch2/catch_test_macros.hpp>
#include <memory>
#include "core/thread_pool.h"

TEST_CASE("ThreadPool Test", "[core/thread_pool]") {
  using namespace pine;
  int pool_size = 8;
  auto thread_pool = std::make_unique<ThreadPool>(pool_size);
  REQUIRE(thread_pool->Size() == pool_size);

  SECTION("lanuch multiple tasks and wait for exit") {
    std::atomic<int> var = 0;
    {
      ThreadPool local_pool(pool_size);
      for (int i = 0; i < 3 * pool_size; i++) {
        local_pool.SubmitTask([&]() { var++; });
      }
      // here thread_pool's dtor should finish all the tasks
    }
    CHECK(var == 3 * pool_size);
  }
}