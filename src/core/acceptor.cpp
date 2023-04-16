#include "core/acceptor.h"
#include "core/connection.h"
#include "core/net_address.h"
#include "core/poller.h"
#include "core/socket.h"
#include "core/looper.h"
#include <cstdlib>
#include <memory>
#include <utility>

namespace pine {

Acceptor::Acceptor(Looper *listener, std::vector<Looper *> reactors, NetAddress server_address)
    : reactors_(std::move(reactors)) {
  auto acceptor_sock = std::make_unique<Socket>();
  acceptor_sock->Bind(server_address, true);
  acceptor_sock->Listen();
  acceptor_conn_ = std::make_unique<Connection>(std::move(acceptor_sock));
  acceptor_conn_->SetEvents(POLL_READ);  // not edge-trigger for listener
  acceptor_conn_->SetLooper(listener);
  listener->AddAcceptor(acceptor_conn_.get());
  SetCustomAcceptCallback([](Connection *) {});
  SetCustomHandleCallback([](Connection *) {});
}

/*
 * basic functionality for accepting new connection
 * provided to the acceptor by default
 */
void Acceptor::ProcessAccept(Connection *server_conn) {
  NetAddress client_address;
  int accept_fd = server_conn->GetSocket()->Accept(client_address);
  if (accept_fd == -1) {
    return;
  }
  auto client_sock = std::make_unique<Socket>(accept_fd);
  client_sock->SetNonBlocking();
  auto client_connection = std::make_unique<Connection>(std::move(client_sock));
  client_connection->SetEvents(POLL_READ | POLL_ET);  // edge-trigger for client
  client_connection->SetCallback(GetCustomHandleCallback());
  // randomized distribution. uniform in long term.
  int idx = rand() % reactors_.size();  // NOLINT
  LOG_INFO("new client fd=" + std::to_string(client_connection->GetFd()) + " maps to reactor " + std::to_string(idx));
  client_connection->SetLooper(reactors_[idx]);
  reactors_[idx]->AddConnection(std::move(client_connection));
}

void Acceptor::SetCustomAcceptCallback(std::function<void(Connection *)> custom_accept_callback) {
  custom_accept_callback_ = std::move(custom_accept_callback);
  acceptor_conn_->SetCallback([this](auto &&PH1) {  // register connection callback.
    ProcessAccept(std::forward<decltype(PH1)>(PH1));
    custom_accept_callback_(std::forward<decltype(PH1)>(PH1));
  });
}

void Acceptor::SetCustomHandleCallback(std::function<void(Connection *)> custom_handle_callback) {
  custom_handle_callback_ = std::move(custom_handle_callback);
}

}