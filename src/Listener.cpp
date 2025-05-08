#include "Listener.h"
#include "Session.h"

Listener::Listener(
    net::io_context& ioc,
    tcp::endpoint endpoint)
    : m_ioc(ioc)
    , m_acceptor(net::make_strand(ioc))
{
    beast::error_code ec;

    // Open the acceptor
    m_acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        fail(ec, "open failure");
        return;
    }

    // Allow address reuse
    m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        fail(ec, "set_option failure");
        return;
    }

    // Bind to the server address
    m_acceptor.bind(endpoint, ec);
    if (ec)
    {
        fail(ec, "bind failure");
        return;
    }

    // Start listening for connections
    m_acceptor.listen(
        net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        fail(ec, "listen failure");
        return;
    }
}

void Listener::do_accept() {
    // Do an async accept on an incoming connnection
    m_acceptor.async_accept(
        net::make_strand(m_ioc),
        beast::bind_front_handler(
            &Listener::on_accept,
            shared_from_this()));
}

void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        fail(ec, "accept");
        return; // To avoid infinite loop
    }
    else {
        std::cout << "Connnection Successful" << '\n';
        // Create the session and run it
        std::make_shared<Session>(std::move(socket))->run();
    }

    // Accept another connection
    do_accept();
}