/**
 * @file request.h
 * @brief 
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include "http/http_utils.h"
#include <ostream>
#include <string>
#include <vector>
#include "http/header.h"

namespace pine::http {

enum class Method;
enum class Version;

class Request {
public:
  // method, version, and resource constructor
  explicit Request(Method method, Version version, std::string resource_url, const std::vector<Header> &headers)
    : method_(method), version_(version), resource_url_(std::move(resource_url)), headers_(headers), valid_(true) {}
  // the request string including method, version, resource
  explicit Request(const std::string &req_str) noexcept;
  Request(const Request&) = delete;
  void operator=(const Request&) = delete;

  bool Valid() const noexcept { return valid_; }
  bool ShouldClose() const noexcept { return should_close_; }
  Method GetMethod() const noexcept { return method_; }
  Version GetVersion() const noexcept { return version_; }
  std::vector<Header> GetHeaders() const noexcept { return headers_; }
  std::string GetResourceUrl() const noexcept { return resource_url_; }
  std::string GetInvalidReason() const noexcept { return invalid_reason_; }

  friend std::ostream & operator << (std::ostream &os, const Request &req); 
private:
  Method method_;
  Version version_;
  std::string resource_url_;
  std::vector<Header> headers_;
  bool valid_ {false};
  bool should_close_ {true};
  std::string invalid_reason_;

  // 检查连接是否存活，如果没有存活，则将should_close设置为true, header参数是传入连接一个键值对
  void ScanHeader(const Header &header);
  // 解析请求行, e.g."GET xxx xxx"
  bool ParseRequestLine(const std::string &req_line); 
};

}