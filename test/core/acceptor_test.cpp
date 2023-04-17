#include "core/acceptor.h"
#include "core/connection.h"
#include "core/net_address.h"
#include "core/looper.h"
#include "core/thread_pool.h"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <memory>
#include <future>
#include <thread>
#include <unistd.h>

using namespace pine;

TEST_CASE("Acceptor Test1", "[core/acceptor]") {
  
  NetAddress local_host("127.0.0.1", 10001);

  // built an acceptor will one listener looper and one reactor together.
  auto single_reactor = std::make_unique<Looper>();

  std::vector<Looper *> raw_reactors = {single_reactor.get()};
  auto acceptor = Acceptor(single_reactor.get(), raw_reactors, local_host);

  REQUIRE(acceptor.GetAcceptorConnection()->GetFd() != -1);

  SECTION(
      "Acceptor should be able to accept new clients and set callback for "
      "them") {
    int client_num = 3;
    std::atomic<int> accept_trigger = 0;
    std::atomic<int> handle_trigger = 0;

    // set acceptor customize functions
    acceptor.SetCustomAcceptCallback([&](Connection *) { accept_trigger++; });
    acceptor.SetCustomHandleCallback([&](Connection *) { handle_trigger++; });

    // start three clients and connect with server
    const char *msg = "Hello from client!";
    std::vector<std::thread> threads;
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&]() {
        Socket client_sock;
        client_sock.Connect(local_host);
        CHECK(client_sock.GetFd() != -1);
        send(client_sock.GetFd(), msg, strlen(msg), 0);
      });
    }

    sleep(2);
    std::cout << "starting to execute runner\n";
    std::thread run_reactor([&]{ single_reactor->Loop(); });
    sleep(5);
    // terminate the looper
    single_reactor->SetExit();
    

    run_reactor.join();
    for (auto &t : threads) {
      t.join();
    }

    // accept & handle should be triggered exactly 3 times
    CHECK(accept_trigger == client_num);
    CHECK(handle_trigger >= client_num);
  }
}

TEST_CASE("Acceptor Test2", "[core/acceptor]") {
  NetAddress local_host("127.0.0.1", 10001);

  // build a acceptor with a listener and a single reactor
  auto listener = std::make_unique<Looper>();
  auto single_reactor = std::make_unique<Looper>();
  std::vector<Looper *> raw_reactors {single_reactor.get()};

  auto acceptor = Acceptor(listener.get(), raw_reactors, local_host);
  CHECK(acceptor.GetAcceptorConnection()->GetFd() != -1);

    std::atomic<int> accept_trigger = 0;
    std::atomic<int> handle_trigger = 0;
  acceptor.SetCustomAcceptCallback([&](Connection *conn) {
    accept_trigger++;
  });
  acceptor.SetCustomHandleCallback([&](Connection *conn) {
    handle_trigger++;
  });

  SECTION("async") {
    int client_num = 3;
    std::vector<std::thread> threads;
    std::string_view msg {"Hello from client!"};
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&]{
        Socket client_sock;
        client_sock.Connect(local_host);
        CHECK(client_sock.GetFd() != - 1);
        send(client_sock.GetFd(), msg.data(), msg.size(), 0);
      });
    }
    sleep(2);

    std::cout << "start to run\n";
    std::thread listener_runner([&] { listener->Loop(); });
    std::thread reactor_runner([&]{ single_reactor->Loop(); });
    sleep(2);
    listener->SetExit();
    single_reactor->SetExit();
    listener_runner.join();
    reactor_runner.join();
    for (auto &t : threads) {
      t.join();
    }
    REQUIRE(accept_trigger == 3);
    REQUIRE(handle_trigger == 3);
  }

  SECTION("apply Thread Pool into Looper") {
    // auto pool = std::make_unique<ThreadPool>(2);
    // auto listener = std::make_unique<Looper>();
    // std::vector<std::unique_ptr<Looper>> reactors;
    // for (int i = 0; i < pool->Size(); i++) {
    //   reactors.emplace_back(std::make_unique<Looper>());
    // }
    // std::vector<Looper *> raw_reactors;

  }
}