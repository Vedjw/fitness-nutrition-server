#pragma once

#include "server_constants.h"
#include "handle_request_functions.h"
#include "User.h"
#include "doc_root.h"


std::string path_cat(
    beast::string_view base,
    beast::string_view path)
{
    if (base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto& c : result)
        if (c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

template <class Body, class Allocator>
http::message_generator handle_file_request(
    http::request<Body, http::basic_fields<Allocator>>&& req
) {
    bool cookie_flag;
    std::cout << req << '\n';

    beast::error_code ec;
    http::file_body::value_type body;
    std::string path = path_cat(doc_root, req.target());
    std::cout << path << '\n';

    if (req.target().back() == '/') {
        path.append("HTML/index.html");
        cookie_flag = true;
    }
    //path = req.target();
    std::cout << path << '\n';

    body.open(path.c_str(), beast::file_mode::scan, ec);

    if (ec == beast::errc::no_such_file_or_directory)
        return not_found(path, std::move(req));

    // Handle an unknown error
    if (ec)
        return server_error(ec.message(), std::move(req));

    // Cache the size since we need it after the move
    auto const size = body.size();

    http::response<http::file_body> res{
       std::piecewise_construct,
       std::make_tuple(std::move(body)),
       std::make_tuple(http::status::ok, req.version()) };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));

    if (cookie_flag) {
        std::string ssid = generate_cookie();
        std::string cookie_str = "session_id=" + ssid;
        res.set(http::field::set_cookie, cookie_str);
    }

    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;

}

template <class Body, class Allocator>
http::message_generator handle_get(
    http::request<Body, http::basic_fields<Allocator>>&& req
) {

    if (req[http::field::content_type] == "text/plain") {
        return handle_get_data(std::move(req));
    }

    return handle_file_request(std::move(req));

};
