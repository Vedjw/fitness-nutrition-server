#include "Session.h"
#include "handle_request.h"
#include <memory>

void Session::run() {

    // thread safety, strand stuff
    net::dispatch(m_stream.get_executor(),
        beast::bind_front_handler(
            &Session::do_read,
            shared_from_this()));
}

void Session::do_read() {
    m_request = {};

    http::async_read(m_stream, m_buffer, m_request,
        beast::bind_front_handler(
            &Session::on_read,
            shared_from_this()));
}

void Session::on_read(
    beast::error_code ec,
    std::size_t bytes_transferred
) {
    boost::ignore_unused(bytes_transferred);

    if (ec == http::error::end_of_stream) {
        return do_close();
    }

    if (ec) {
        return fail(ec, "read");
    }

    send_response(
        handle_request(
            std::move(m_request)));

}

void Session::send_response(http::message_generator&& msg) {
    // To decide if the connection should be kept alive for
    // further reads
    bool keep_alive = msg.keep_alive();

    // Write the response
    beast::async_write(
        m_stream,
        std::move(msg),
        beast::bind_front_handler(
            &Session::on_write, shared_from_this(), keep_alive));
}

void Session::on_write(
    bool keep_alive,
    beast::error_code ec,
    size_t bytes_transferred
) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        return fail(ec, "write");
    }
    if (!keep_alive) {
        std::cout << "2" << '\n';
        return do_close();
    }

    do_read();
}

void Session::do_close() {
    // Send a TCP shutdown
    beast::error_code ec;
    m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
    // At this point the connection is closed gracefully
}