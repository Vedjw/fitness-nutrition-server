#pragma once

#include "server_constants.h"
#include "User.h"
#include <memory>

class Session : public std::enable_shared_from_this<Session>
{
    beast::tcp_stream m_stream;
    beast::flat_buffer m_buffer;
    http::request<http::string_body> m_request;

public:

    Session(tcp::socket&& socket)
        : m_stream{ std::move(socket) }
    {
    }

    ~Session()
    {
        std::cout << "Session destroyed" << '\n';
    }

    void run();

    void do_read();

    void on_read(
        beast::error_code ec,
        size_t bytes_transfered);

    void send_response(http::message_generator&& msg);

    void on_write(
        bool keep_alive,
        beast::error_code ec,
        size_t bytes_transfered);

    void do_close();
};