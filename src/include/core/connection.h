/**
 * @file connection.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "buffer.h"
#include "socket.h"
#include <cstdint>
#include <memory>
#include <functional>
#include <optional>
#include <sys/types.h>
#include <utility>

namespace pine {

class Looper;

class Connection {
public:
  // Connection();
  
  explicit Connection(std::unique_ptr<Socket> socket) noexcept
    : socket_(std::move(socket)), read_buffer_(std::make_unique<Buffer>()), write_buffer_(std::make_unique<Buffer>()) {}

  ~Connection() = default;

  auto GetFd() const -> int { return socket_->GetFd(); }
  
  auto GetSocket() const noexcept -> Socket * { return socket_.get(); } 

  /* for poller */
  auto GetEvents() const -> uint32_t { return events_; }
  
  void SetEvents(uint32_t events) { events_ = events; }
  
  auto GetRevents() const -> uint32_t { return revents_; }
  
  void SetRevents(uint32_t revents) { revents_ = revents; }

  /* callback function */
  auto GetCallback() const -> std::function<void()> {
    return callback_;
  }
  
  void SetCallback(const std::function<void(Connection *)> &callback) {
    callback_ = [this, callback](){ return callback(this); };
  }

  void SetCallback(std::function<void(Connection *)> &&callback) noexcept {
    callback_ = [this, callback] {return callback(this); };
  }

  /* for Buffer */
  auto ReadBufferSize() const noexcept -> int { return read_buffer_->Size(); }
  
  auto WriteBufferSize() const noexcept -> int { return write_buffer_->Size(); }
  
  auto FindAndPopTill(const std::string &target) -> std::optional<std::string> {
    return read_buffer_->FindAndPopTill(target);
  }

  void WriteToReadBuffer(const char *data, size_t data_size) {
    read_buffer_->Append(data, data_size);
  }

  void WriteToReadBuffer(const std::string &data) {
    read_buffer_->Append(data);
  }

  void WriteToReadBuffer(std::string_view data) noexcept {
    read_buffer_->Append(data.data(), data.size());
  }

  void WriteToReadBuffer(std::vector<char> &&data) noexcept {
    read_buffer_->Append(std::move(data));
  }

  void WriteToWriteBuffer(const char *data, size_t data_size) {
    write_buffer_->Append(data, data_size);
  }

  void WriteToWriteBuffer(std::string_view data) noexcept {
    write_buffer_->Append(data.data(), data.size());
  }

  void WriteToWriteBuffer(const std::string &data) {
    write_buffer_->Append(data);
  }

  void WriteToWriteBuffer(std::vector<char> &&data) noexcept {
    write_buffer_->Append(std::move(data));
  }

  auto Read() const noexcept -> const char * {
    return read_buffer_->Data();
  }

  auto ReadAsString() const noexcept -> std::string {
    return std::string(read_buffer_->ToStringView());
  }

  auto Recv() -> std::pair<ssize_t, bool>;

  void Send();

  void ClearReadBuffer() noexcept {
    read_buffer_->Clear();
  }

  void ClearWriteBuffer() noexcept {
    write_buffer_->Clear();
  }
  
  /* looper */
  auto GetLooper() const -> Looper * {
    return owner_looper_;
  }

  auto SetLooper(Looper *looper) {
    owner_looper_ = looper;
  }

private:
  Looper *owner_looper_ {nullptr};
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Buffer> read_buffer_;
  std::unique_ptr<Buffer> write_buffer_;
  uint32_t events_ {0};
  uint32_t revents_ {0};
  std::function<void()> callback_; 
};

}