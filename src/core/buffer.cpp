#include "core/buffer.h"
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>

namespace pine {

Buffer::Buffer(const Buffer & other) noexcept {
  if (&other == this) return;
  std::copy(std::begin(other.buffer_), std::end(other.buffer_), std::back_inserter(buffer_));
}


auto Buffer::operator = (const Buffer & other) noexcept -> Buffer & {
  if (&other == this) {
    return *this;
  }
  std::copy(std::begin(other.buffer_), std::end(other.buffer_), std::back_inserter(buffer_));
  return *this;
}

void Buffer::Append(const char *data, size_t data_size) {
  buffer_.insert(buffer_.end(), data, data + data_size);
}

void Buffer::Append(const std::string &data) {
  Append(reinterpret_cast<const char *>(data.data()), data.size());
}

void Buffer::Append(std::vector<char> &&buf) noexcept {
  buffer_.insert(buffer_.end(), std::make_move_iterator(buf.begin()), std::make_move_iterator(buf.end()));
}

void Buffer::AppendHead(const char *data, size_t data_size) {
  buffer_.insert(buffer_.begin(), data, data + data_size);
}

void Buffer::AppendHead(const std::string &data) {
  AppendHead(reinterpret_cast<const char *>(data.data()), data.size());
}

auto Buffer::FindAndPopTill(const std::string &target) -> std::optional<std::string> {
  std::optional<std::string> res =  std::nullopt;
  auto buffer_view = ToStringView();
  if (auto pos = buffer_view.find(target); pos != std::string::npos) {
    res = buffer_view.substr(0, pos + target.size());
    // auto it = std::begin(buffer_);
    // std::advance(it, pos + target.size());
    buffer_.erase(buffer_.begin(), buffer_.begin() + pos + target.size());
  }
  return res;
}

}