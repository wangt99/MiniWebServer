#include "core/acceptor.h"
#include "core/connection.h"
#include "core/net_address.h"
#include "core/looper.h"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <memory>
#include <future>
#include <thread>
#include <unistd.h>

using namespace pine;

TEST_CASE("[core/acceptor]") {
  
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