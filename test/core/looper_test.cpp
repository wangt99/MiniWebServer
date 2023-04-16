#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <numeric>
#include <unistd.h>
#include <algorithm>
#include "core/connection.h"
#include "core/looper.h"
#include "core/net_address.h"
#include "core/poller.h"
#include "core/socket.h"

TEST_CASE("Looper Test", "[core/looper]") {
  using namespace pine;

  Looper looper;
  // build the sever socket
  NetAddress local_host("127.0.0.1", 10008);
  auto server_sock = std::make_unique<Socket>();
  server_sock->Bind(local_host);
  server_sock->Listen();
  CHECK(server_sock->GetFd() != -1);

  SECTION("looper execute connection's callback function when triggerd") {
    int client_num = 3;
    std::vector<std::thread> threads;
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&](){
        Socket client_sock;
        client_sock.Connect(local_host);
        sleep(1);
      });
    }

    std::vector<int> reach(client_num, 0);
    for (int i = 0; i < client_num; i++) {
      NetAddress client_address;
      auto client_socket = std::make_unique<Socket>(server_sock->Accept(client_address));
      client_socket->SetNonBlocking();
      auto client_conn = std::make_unique<Connection>(std::move(client_socket));
      client_conn->SetEvents(POLL_READ);
      client_conn->SetCallback([&reach, index=i](Connection *){
        reach[index] = 1;
      });
      looper.AddConnection(std::move(client_conn));
    }

    std::thread runner([&]{ looper.Loop(); });
    sleep(2);
    looper.SetExit();

    CHECK(std::accumulate(reach.begin(), reach.end(), 0) == client_num);
    runner.join();
    for (auto &t : threads) {
      t.join();
    }
  }
}