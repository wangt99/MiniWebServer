#pragma once

#include "http/http_utils.h"
#include <iostream>
#include <string>
#include <string_view>

namespace pine::http {
/*
A http header class, which is essentially including key-value pair. 
*/
class Header {
public:
  Header(const std::string &key, const std::string &value) : key_(key), value_(value) {} 
  Header(std::string &&key, std::string &&value): key_(std::move(key)), value_(std::move(value)) {}
  // parse line into key and value
  explicit Header(const std::string &line) noexcept;
  Header(const Header &other) = default;
  auto operator = (const Header &other) -> Header & = default;
  Header(Header &&other) noexcept;
  auto operator = (Header &&other) noexcept -> Header &;

  auto Key() const -> std::string { return key_;}

  auto Value() const -> std::string { return value_; }

  auto IsValid() const -> bool { return is_valid_; }

  void SetValue(const std::string &value) { value_ = value; }

  auto Serialize() const -> std::string { return key_ + COLON + value_ + CRLF; }

private:
  std::string key_;
  std::string value_;
  bool is_valid_ {true};

  void ParseLine(const std::string &line) noexcept;
};

}