#pragma once

#include "server_constants.h"
#include "Session.h"

http::response<http::string_body> bad_request(
    const std::string_view& why,
    http::request<http::string_body>&& req);

http::response<http::string_body> server_error(
    const std::string_view& why,
    http::request<http::string_body>&& req);

http::response<http::string_body> not_found(
    const std::string_view& why,
    http::request<http::string_body>&& req);

beast::string_view mime_type(beast::string_view path);

http::response<http::string_body> handle_post(
    http::request<http::string_body>&& req
);

http::response<http::string_body> handle_get_data(
    http::request<http::string_body>&& req
);

std::string generate_cookie(); // optimize to string_view
std::string get_cookie(std::string& cookie_string);