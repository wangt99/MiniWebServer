#pragma once
#include <cassert>
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>
#include <string>
#include <cstring>
#include <map>

namespace pine::http {

constexpr int READ_WRITE_PERM = 0600;

constexpr char PARAMETER_SEPARATOR[] = {"&"};
constexpr char UNDERSCORE[] = {"_"};
constexpr char SPACE[] = {" "};
constexpr char DOT[] = {"."};
constexpr char CRLF[] = {"\r\n"};
constexpr char COLON[] = {":"};
constexpr char DEFAULT_ROUTE[] = {"index.html"};
constexpr char CGI_BIN[] = {"cgi-bin"};
constexpr char CGI_PREFIX[] = {"cgi_temp"};

/// Common head and value
constexpr char HEADER_SERVER[] = {"Server"};
constexpr char SERVER_NAME[] = {"PINE/0.1"};
constexpr char HEADER_CONTENT_LENGTH[] = {"Content-Length"};
constexpr char HEADER_CONTENT_TYPE[] = {"Content-Type"};
constexpr char CONTENT_LENGTH_ZERO[] = {"0"};
constexpr char HEADER_CONNECTION[] = {"Connection"};
constexpr char CONNECTION_CLOSE[] = {"Close"};
constexpr char CONNECTION_KEEP_ALIVE[] = {"Keep-Alive"};
constexpr char HTTP_VERSION[] = {"HTTP/1.1"};

/// MIME Type
constexpr char MIME_HTML[] = {"text/html"};
constexpr char MIME_CSS[] = {"text/css"};
constexpr char MIME_PNG[] = {"image/png"};
constexpr char MIME_JPG[] = {"image/jpg"};
constexpr char MIME_JPEG[] = {"image/jpeg"};
constexpr char MIME_GIF[] = {"image/gif"};
constexpr char MIME_OCTET[] = {"application/octet-stream"};

/// Response Status
constexpr char RESPONSE_OK[] = {"200 OK"};
// constexpr char RESPONSE_REDIRECTION[] = {"303 OK"};
constexpr char RESPONSE_BAD_REQUEST[] = {"400 Bad Request"};
constexpr char RESPONSE_NOT_FOUND[] = {"404 Not Found"};
constexpr char RESPONSE_SERVICE_UNAVAILABLE[] = {"503 Service Unavailable"};


enum class Method {
  UNSUPPORT = -1,
  GET,
  POST,
};

/* HTTP version enum, only support HTTP 1.1 now */
enum class Version { HTTP_1_1, UNSUPPORT };

/* Content Extension enum */
enum class Extension { HTML, CSS, PNG, JPG, JPEG, GIF, OCTET };

const std::map<Method, std::string> Method2String{
  {Method::GET, "GET"},
  {Method::POST, "POST"},
};

const std::map<Extension, std::string> Extension2String{
  {Extension::HTML, "HTML"},
  {Extension::CSS, "CSS"},
  {Extension::PNG, "PNG"},
  {Extension::JPG, "JPG"},
  {Extension::JPEG, "JEPG"},
  {Extension::GIF, "GIF"},
  {Extension::OCTET, "OCTET"},
};


std::vector<std::string> Split(const std::string &str, const char *delim);
std::string Join(const std::vector<std::string> &tokens, const char *delim);
std::string Trim(const std::string &str, const char *delim = SPACE);
std::string ToUpper(const std::string &str);
std::string Format(const std::string &str);

Method ToMethod(const std::string &method);
Version ToVersion(const std::string &version);
Extension ToExtension(const std::string &extension_str);
const char* Extension2Mime(Extension ext);

// File process
inline bool FileExists(std::string_view file_path) {
  return std::filesystem::exists(file_path);
}

inline size_t FileSize(std::string_view file_path) {
  assert(FileExists(file_path));
  return std::filesystem::file_size(file_path);
}

inline void LoadFile(const std::string &file_path, std::vector<unsigned char> &buffer) {
  size_t file_size = FileSize(file_path);
  size_t old_size = buffer.size();
  buffer.resize(file_size + old_size);
  std::ifstream fs(file_path);
  assert(fs.good());
  fs.read(reinterpret_cast<char*>(&buffer[old_size]), static_cast<std::streamsize>(file_size));
}

inline void DeleteFile(const std::string &file_path) {
  std::filesystem::remove(file_path);
}

// process cgi program 
inline bool IsCgiRequest(std::string_view resource_url) {
  return resource_url.find(CGI_BIN) != std::string::npos;
}

}