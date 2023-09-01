#include "http/header.h"
#include "http/http_utils.h"
#include <ostream>

namespace pine::http {

void Header::ParseLine(const std::string &line) noexcept {
  auto tokens = Split(line, COLON);
  if (tokens.size() < 2) {
    is_valid_ = false;
    return;
  }
  key_ = std::move(tokens[0]);
  tokens.erase(tokens.begin());
  std::string value = (tokens.size() == 1) ? tokens[0] : Join(tokens, COLON);
  value_ = std::move(value);
}

Header::Header(const std::string &line) noexcept {
  ParseLine(line);
}

Header::Header(Header &&other) noexcept
  : key_(std::move(other.key_)), value_(std::move(other.value_)), is_valid_(other.is_valid_) {}

auto Header::operator = (Header &&other) noexcept -> Header& {
  key_ = std::move(other.key_);
  value_ = std::move(other.value_);
  is_valid_ = other.is_valid_;
  return *this;
}

auto operator << (std::ostream &os, const Header &header) -> std::ostream & {
  os << "HTTP Header: \n";
  os << "Key: " << header.Key() << '\n';
  os << "Value: " << header.Value() << '\n';
  os << "IsValid: " << (header.IsValid() ? "True" : "False") << '\n';
  os << "Serialize to: " << header.Serialize();
  return os;
}

}