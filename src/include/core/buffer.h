/**
 * @file buffer.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once


#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <string_view>



namespace pine {

constexpr size_t INTIAL_BUFFER_SIZE = 1024;

/**
 * @brief char buffer
 * 
 */
class Buffer {
public:
  explicit Buffer(size_t size = INTIAL_BUFFER_SIZE) {
    buffer_.reserve(size);
  }

  Buffer(const Buffer & other) noexcept;

  auto operator = (const Buffer & other) noexcept -> Buffer &;

  ~Buffer() = default;

  void Append(const char * data, size_t data_size);

  void Append(const std::string & data);

  void Append(std::vector<char> && buf) noexcept;

  void AppendHead(const char * data, size_t data_size);

  void AppendHead(const std::string & data);

  auto FindAndPopTill(const std::string & target) -> std::optional<std::string>;

  auto Size() const noexcept -> int { return buffer_.size(); }

  
  auto Capacity() const noexcept -> int { return buffer_.capacity(); }

  // auto Data() noexcept const -> const unsigned char * { return buffer_.data(); }
  auto Data() noexcept -> const char * { return buffer_.data(); }

  void Clear() noexcept { buffer_.clear(); }

  auto ToStringView() const noexcept-> std::string_view {
    return {reinterpret_cast<const char *>(buffer_.data()), buffer_.size()};
  }

private:
  std::vector<char> buffer_;
  
};


}