#include "http/http_utils.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <sstream>

namespace pine::http {

std::vector<std::string> Split(const std::string &str, const char *delim) {
  std::vector<std::string> tokens;
  if (str.empty()) {
    return tokens;
  }

  size_t curr = 0;
  size_t next;
  size_t delim_len = strlen(delim);
  while((next = str.find(delim, curr)) != std::string::npos) {
    tokens.push_back(str.substr(curr, next - curr));
    curr = next + delim_len;
  }
  if (curr != str.size()) {
    // one last word
    tokens.push_back(str.substr(curr, str.size() - curr));
  }
  return tokens;
}

std::string Join(const std::vector<std::string> &tokens, const char *delim) {
  if (tokens.empty()) {
    return {};
  }
  if (tokens.size() == 1) {
    return tokens[0];
  }

  std::stringstream str_stream;
  size_t len = tokens.size();
  for (size_t i = 0; i < len - 1; i++) {
    str_stream << tokens[i] << delim;
  }
  str_stream << tokens[len - 1];
  return str_stream.str();
}

std::string Trim(const std::string &str, const char *delim) {
  size_t r_found = str.find_last_not_of(delim);
  if (r_found == std::string::npos) {
    return {};
  }
  size_t l_found = str.find_first_not_of(delim);
  return str.substr(l_found, r_found - l_found + 1);
}

std::string ToUpper(const std::string &str) {
  std::string ret;
  ret.reserve(str.size());
  std::transform(str.begin(), str.end(), std::back_inserter(ret),[](unsigned char c) {
    return std::toupper(c);
  });
  return ret;
}

std::string Format(const std::string &str) {
  return ToUpper(Trim(str));
}

Method ToMethod(const std::string &method) {
  std::string method_str = Format(method);
  if (method_str == Method2String.at(Method::GET)) {
    return Method::GET;
  }

  if (method_str == Method2String.at(Method::POST)) {
    return Method::POST;
  }
  return Method::UNSUPPORT;
}

Version ToVersion(const std::string &version) {
  std::string version_str = Format(version);
  if (version_str == "HTTP/1.1") {
    return Version::HTTP_1_1;
  }
  return Version::UNSUPPORT;
}

Extension ToExtension(const std::string &extension_str) {
  auto extension = Format(extension_str);
  if (extension == Extension2String.at(Extension::HTML)) {
    return Extension::HTML;
  }

  if (extension == Extension2String.at(Extension::CSS)) {
    return Extension::CSS;
  }

  if (extension == Extension2String.at(Extension::PNG)) {
    return Extension::PNG;
  }

  if (extension == Extension2String.at(Extension::JPG)) {
    return Extension::JPG;
  }

  if (extension == Extension2String.at(Extension::JPEG)) {
    return Extension::JPEG;
  }

  if (extension == Extension2String.at(Extension::GIF)) {
    return Extension::GIF;
  }
  return Extension::OCTET;
}

const char* Extension2Mime(Extension extension) {
  if (extension == Extension::HTML) {
    return MIME_HTML;
  }

  if (extension == Extension::CSS) {
    return MIME_CSS;
  }

  if (extension == Extension::PNG) {
    return MIME_PNG;
  }

  if (extension == Extension::JPG) {
    return MIME_JPG;
  }

  if (extension == Extension::JPEG) {
    return MIME_JPEG;
  }

  if (extension == Extension::GIF) {
    return MIME_GIF;
  }
  return MIME_OCTET;
}

}