/**
 * @file response.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "http/header.h"
#include <optional>
namespace pine::http {

class Response {
public:
    Response(std::string status, bool should_close, std::optional<std::string> resource_url);

    static Response Make200Response(bool should_close, std::optional<std::string> resource_url) noexcept;
    static Response Make404Response() noexcept;
    static Response Make400Response() noexcept;
    static Response Make503Response() noexcept;

    void Serialize(std::vector<unsigned char> &buffer);

    auto GetHeaders() const -> const std::vector<Header> & { return headers_; }

    bool ChangeHeader(const std::string &key, const std::string &value) noexcept;

private:
    std::string status_;
    bool should_close_;
    std::optional<std::string> resource_url_;
    std::vector<Header> headers_;
    std::vector<unsigned char> body_;
};


}