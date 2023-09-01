#include "http/request.h"
#include "http/header.h"
#include "http/http_utils.h"
#include <ostream>

namespace pine::http {

Request::Request(const std::string &req_str) noexcept {
  auto lines = Split(req_str, CRLF);
  if (lines.size() < 2 || !lines.back().empty()) {
    invalid_reason_ = "Request format is invalid";
    return;
  }
  // the ending of request should be '\r\n\r\n'
  if (!lines.back().empty()) {
    invalid_reason_ = "Ending of the request is not \r\n\r\n";
    return;
  }
  lines.pop_back();
  bool req_line_parse_success = ParseRequestLine(lines[0]);
  if (!req_line_parse_success) {
    invalid_reason_ = "Fail to parse request line: " + lines[0];
    return;
  }
  lines.erase(lines.begin());
  for (auto const &line: lines) {
    Header header {line};
    if (!header.IsValid()) {
      invalid_reason_ = "Fail to parse header line: " + line;
      return;
    }
    ScanHeader(header);
    headers_.emplace_back(std::move(header));
  }
  valid_ = true;
}

void Request::ScanHeader(const Header &header) {
  auto key = Format(header.Key());
  if (key == Format(HEADER_CONNECTION)) {
    auto value = Format(header.Value());
    if (value == Format(CONNECTION_KEEP_ALIVE)) {
      should_close_ = false;
    }
  }
}

// "GET xxx <version>"
bool Request::ParseRequestLine(const std::string &req_line) {
  auto tokens = Split(req_line, SPACE);
  if (tokens.size() != 3) {
    invalid_reason_ = "Invalid first request headline: " + req_line;
    return false;
  }

  method_ = ToMethod(tokens[0]);
  if (method_ == Method::UNSUPPORT) {
    invalid_reason_ = "Unsupport request method: " + tokens[0];
    return false;
  }
  version_ = ToVersion(tokens[2]);
  if (version_ == Version::UNSUPPORT) {
    invalid_reason_ = "Unsupport version: " + tokens[2];
    return false;
  }
  // default route to index.html
  resource_url_ = (tokens[1].empty() || tokens[1].at(tokens[1].size() - 1) == '/') ? tokens[1] + DEFAULT_ROUTE : tokens[1];
  return true;
}

auto operator << (std::ostream &os, const Request &req) -> std::ostream & {
  if (req.Valid()) {
    os << "Reqeust is valid.\n";
    os << "Method: " << Method2String.at(req.method_) << '\n';
    os << "HTTP Version: HTTP1.1\n";
  } else {
    os << "Request is not invalid." << std::endl;
    os << "Reason: " << req.invalid_reason_ << std::endl;
  }
  return os;
}

}