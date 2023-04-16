/**
 * @file acceptor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "core/connection.h"
#include "core/net_address.h"
#include "core/socket.h"
#include <functional>
#include <memory>
#include <vector>


namespace pine {

/**
 * This Acceptor comes with basic functionality for accepting new client
 * connections and distribute its into the different Poller.
 * More custom handling could be added as well
 */
class Acceptor {
 public:
  Acceptor(Looper *listener, std::vector<Looper *> reactors, NetAddress server_address);

  ~Acceptor() = default;

  void ProcessAccept(Connection *server_conn);

  void SetCustomAcceptCallback(std::function<void(Connection *)> custom_accept_callback);

  void SetCustomHandleCallback(std::function<void(Connection *)> custom_handle_callback);

  auto GetCustomAcceptCallback() const noexcept -> std::function<void(Connection *)> {
    return custom_accept_callback_;
  }

  auto GetCustomHandleCallback() const noexcept -> std::function<void(Connection *)> {
    return custom_handle_callback_;
  }

  auto GetAcceptorConnection() noexcept -> Connection * {
    return acceptor_conn_.get();
  }

 private:
  std::vector<Looper *> reactors_;
  std::unique_ptr<Connection> acceptor_conn_;
  std::function<void(Connection *)> custom_accept_callback_{};
  std::function<void(Connection *)> custom_handle_callback_{};
};
 
}