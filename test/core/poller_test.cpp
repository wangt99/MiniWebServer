#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>
#include <thread>
#include "core/connection.h"
#include "core/poller.h"
#include "core/socket.h"
#include "core/net_address.h"

TEST_CASE("Poller Test", "[core/poller]") {
  using namespace pine;

  NetAddress local_host("127.0.0.1", 10080);
  Socket server_sock;

  // build the server socket
  server_sock.Bind(local_host);
  server_sock.Listen();

  // build the empty poller
  int client_num = 3;
  Poller poller(3);
  REQUIRE(poller.GetPollSize() == client_num);

  SECTION("poll out the client's message") {
    std::vector<std::thread> threads;
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&] {
        auto client_socket = Socket();
        client_socket.Connect(local_host);
        // std::stringstream ss;
        // ss << "Hello from client! " << std::this_thread::get_id();
        // std::string message = ss.str();
        std::string message {"Hello from clinet!"};
        send(client_socket.GetFd(), message.data(), message.size(), 0);
        sleep(2);
      });
    }

    // server accept clients and build connection for them
    std::vector<std::shared_ptr<Connection>> client_conns;
    for (int i = 0; i < client_num; i++) {
      NetAddress client_address;
      auto client_sock = std::make_unique<Socket>(server_sock.Accept(client_address));
      CHECK(client_sock->GetFd() != -1);
      client_conns.push_back(std::make_shared<Connection>(std::move(client_sock)));
      client_conns[i]->SetEvents(POLL_READ);
    }

    // each client connection under poller's monitor
    for (int i = 0; i < client_num; i++) {
      poller.AddConnection(client_conns[i].get());
    }
    sleep(1);
    auto ready_conns = poller.Poll();
    CHECK(ready_conns.size() == client_num);

    for (int i = 0; i < client_num; i++) {
      threads[i].join();
    }
  }
}