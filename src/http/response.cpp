#include "http/header.h"
#include "http/http_utils.h"
#include <http/response.h>
#include <optional>
#include <sstream>

namespace pine::http {

Response Response::Make200Response(bool should_close, std::optional<std::string> resource_url) noexcept {
    return {RESPONSE_OK, should_close, resource_url};
}

Response Response::Make404Response() noexcept {
    return {RESPONSE_NOT_FOUND, true, std::nullopt};
}

Response Response::Make400Response() noexcept {
        return {RESPONSE_BAD_REQUEST, true, std::nullopt};
}

Response Response::Make503Response() noexcept {
    return {RESPONSE_NOT_FOUND, true, std::nullopt};
}

Response::Response(std::string status, bool should_close, std::optional<std::string> resource_url)
    : status_(std::move(status)), should_close_(should_close), resource_url_(std::move(resource_url)) {
    // construct status line
    std::stringstream ss;
    ss << HTTP_VERSION << SPACE << status;
    status_ = ss.str();
    // 
    headers_.push_back({HEADER_SERVER, SERVER_NAME});
    headers_.push_back({HEADER_CONNECTION, ((should_close) ? CONNECTION_CLOSE : CONNECTION_KEEP_ALIVE)});
    if (resource_url.has_value() && FileExists(resource_url.value())) {
        auto file_size = FileSize(resource_url.value());
        headers_.push_back({HEADER_CONTENT_LENGTH, std::to_string(file_size)});
        // parse file extension
        auto pos = resource_url.value().find_last_of(DOT);
        if (pos != std::string::npos) {
            auto ext_str = resource_url.value().substr(pos+1);
            auto ext = ToExtension(ext_str);
            headers_.push_back({HEADER_CONTENT_TYPE, Extension2Mime(ext)});
        }
    } else {
        resource_url_ = std::nullopt;
        headers_.push_back({HEADER_CONTENT_LENGTH, CONTENT_LENGTH_ZERO});
    }
}

void Response::Serialize(std::vector<unsigned char> &buffer) {
    std::stringstream ss;
    ss << status_ << CRLF;
    for (const auto &header : headers_) {
        ss << header.Serialize();
    }
    ss << CRLF;
    auto response_header = ss.str();
    buffer.insert(buffer.end(), response_header.begin(), response_header.end());
}

bool Response::ChangeHeader(const std::string &key, const std::string &value) noexcept {
    for (auto &header : headers_) {
        if (header.Key() == key) {
            header.SetValue(value);
            return true;
        }
    }
    return false;
}

}