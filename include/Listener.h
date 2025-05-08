#pragma once

#include "server_constants.h"

class Listener : public std::enable_shared_from_this<Listener>
{
    net::io_context& m_ioc;
    tcp::acceptor m_acceptor;
public:
    Listener(net::io_context& ioc,
        tcp::endpoint endpoint);

    void run() {
        do_accept();
    };

private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};