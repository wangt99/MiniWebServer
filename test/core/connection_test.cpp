#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <utility>

#include "core/connection.h"
#include "core/poller.h"
#include "core/socket.h"
#include "core/buffer.h"
#include "core/net_address.h"

TEST_CASE("Connection Test", "[core/connection]") {
  using namespace pine;

  auto local_host = NetAddress("127.0.0.1", 12000);
  auto server_sock = std::make_unique<Socket>();
  server_sock->Bind(local_host);
  server_sock->Listen();

  Connection server_conn(std::move(server_sock));
  REQUIRE(server_conn.GetSocket() != nullptr);
  REQUIRE(server_conn.GetFd() != -1);

  SECTION("connection set events and return events") {
    server_conn.SetEvents(POLL_ET | POLL_ADD);
    CHECK((server_conn.GetEvents() & POLL_ET));
    CHECK((server_conn.GetEvents() & POLL_ADD));
    server_conn.SetRevents(POLL_READ);
    CHECK((server_conn.GetRevents() & POLL_READ));
  }

  SECTION("connection's callback setup and invoke") {
    server_conn.SetCallback([](Connection *) {});
    int i = 0;
    server_conn.SetCallback([&i](Connection *) { i += 1; });
    server_conn.GetCallback()();
    CHECK(i == 1);
  }

  SECTION("connection's send and recv message") {
    std::string_view client_msg {"Hello from client"};
    std::string_view server_msg {"Hello from server"};
    std::thread clinet_thread([&] {
      // build a client connecting with server
      auto client_sock = std::make_unique<Socket>();
      client_sock->Connect(local_host);
      Connection client_conn(std::move(client_sock));
      // send a message to server
      client_conn.WriteToWriteBuffer(client_msg);
      CHECK(client_conn.WriteBufferSize() == client_msg.size());
      client_conn.Send();
      // recv a message from server
      sleep(1);
      auto [read, exit] = client_conn.Recv();
      CHECK((read == server_msg.size() && exit));
      CHECK(client_conn.ReadAsString() == server_msg);
    });

    clinet_thread.detach();

    NetAddress client_address;
    auto connected_sock = std::make_unique<Socket>(server_conn.GetSocket()->Accept(client_address));
    connected_sock->SetNonBlocking();
    CHECK(connected_sock->GetFd() != -1);
    Connection connected_conn(std::move(connected_sock));
    sleep(1);
    // recv a message from client
    auto [read, exit] = connected_conn.Recv();
    CHECK((read == client_msg.size() && !exit));
    CHECK(connected_conn.ReadBufferSize() == client_msg.size());
    CHECK(connected_conn.ReadAsString() == client_msg);
    // send a message to client
    connected_conn.WriteToWriteBuffer(server_msg);
    connected_conn.Send();
    sleep(1);
  }

  SECTION("echo msg from clinet and server") {
    std::string_view msg {"Hello world from clinet."};
    std::thread clinet([&] {
      auto clinet_sock = std::make_unique<Socket>();
      clinet_sock->Connect(local_host);
      REQUIRE(clinet_sock->GetFd() != -1);
      Connection conn(std::move(clinet_sock));
      conn.WriteToWriteBuffer(msg);
      conn.Send();
      sleep(2);
      conn.Recv();
      auto echo_msg = conn.ReadAsString();
      std::cout << "echo msg: " << echo_msg << '\n';
      // CHECK(echo_msg == msg);
    });
    clinet.detach();

    // std::thread server([&] {
    NetAddress client_addr;
    auto connected_sock = std::make_unique<Socket>(server_conn.GetSocket()->Accept(client_addr));
    connected_sock->SetNonBlocking();
    REQUIRE(connected_sock->GetFd() != -1);
    Connection clinet_conn(std::move(connected_sock));
    clinet_conn.Recv();
    auto from_msg = clinet_conn.ReadAsString();
    CHECK(msg == from_msg);
    std::cout << "from msg: " << from_msg << '\n';
    // echo msg to client
    clinet_conn.WriteToWriteBuffer(from_msg);
    clinet_conn.Send();
    // });
    // server.detach();

    // server.join();
  }

}