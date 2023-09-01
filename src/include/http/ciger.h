/**
 * @file ciger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
 
#include <string>
#include <vector>

namespace pine::http {

class Cgier {
public:
  Cgier() = default;
  explicit Cgier(const std::string &path, const std::vector<std::string> &args) noexcept;
  auto Run() -> std::vector<unsigned char>;
  
  std::string GetPath() const noexcept { return cgi_path_; }
  bool IsValid() const noexcept { return valid_; }

  static auto ParseCgier(const std::string &resource_url) noexcept -> Cgier;
  static auto MakeInvalidCgier() noexcept -> Cgier;
  
private:
  std::string cgi_path_{};  // cgi程序的路径
  std::vector<std::string> cgi_arguments_{};  // 传入cgi程序的参数
  bool valid_ {false};

  void FreeArgumentList(char **arg_list);
  auto BuildArgumentList() -> char **;
};

}