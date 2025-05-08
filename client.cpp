//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP client, asynchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void
fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

std::string take_input() {
    using json = nlohmann::json;

    // uint64_t id{};
    // std::string f_name{};
    // std::string l_name{};
    // int gender;
    // int age;
    // float height;
    // float weight;

    // std::cout << "Enter first name" << '\n';
    // std::cin >> f_name;
    // std::cout << "Enter last name" << "\n";
    // std::cin >> l_name;
    // std::cout << "Enter gender" << "\n";
    // std::cin >> gender;
    // std::cout << "Enter age" << "\n";
    // std::cin >> age;
    // std::cout << "Enter height" << '\n';
    // std::cin >> height;
    // std::cout << "Enter weight" << '\n';
    // std::cin >> weight;


    json j;

    // j = { {"User" , {
    //         {"Fullname", "Ved W"},
    //         {"Gender", 1},
    //         {"Age", 21},
    //         {"Height", 1.77f},
    //         {"Weight", 56.0f},
    //         {"Weight_goal", 1},
    //         {"Email", "xyz@mail"},
    //         {"Password", "hilo"}
    //     }}
    // };

    // j = { {"UserAuthInfo", {
    //         {"Email", "xyz@mail"},
    //         {"Password", "hilo"}
    //     }}
    // };

    j = {
        {"Fitness_score", 100},
        {"Calories_consumed", 1000},
        {"Calories_burned", 1100},
        {"BMI", 18.0},
        {"BMR", 1000}
    };

    std::string str = j.dump();

    return str;
}

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

public:
    // Objects are constructed with a strand to
    // ensure that handlers do not execute concurrently.
    explicit
        session(net::io_context& ioc)
        : resolver_(net::make_strand(ioc))
        , stream_(net::make_strand(ioc))
    {
    }

    // Start the asynchronous operation
    void
        run(
            char const* host,
            char const* port,
            std::string_view target,
            int version)
    {
        // Set up an HTTP GET request message


        req_.version(version);
        req_.method(http::verb::post);
        req_.target(target);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req_.set(http::field::content_type, "text/plain");
        req_.body() = take_input();
        req_.set(http::field::set_cookie, "session_id=12345");
        req_.prepare_payload();
        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &session::on_resolve,
                shared_from_this()));
    }

    void
        on_resolve(
            beast::error_code ec,
            tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // Set a timeout on the operation
        stream_.expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        stream_.async_connect(
            results,
            beast::bind_front_handler(
                &session::on_connect,
                shared_from_this()));
    }

    void
        on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
            return fail(ec, "connect");

        // Set a timeout on the operation
        stream_.expires_after(std::chrono::seconds(30));

        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
            beast::bind_front_handler(
                &session::on_write,
                shared_from_this()));
    }

    void
        on_write(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        std::cout << "bytes Transfered: " << bytes_transferred << '\n';

        if (ec)
            return fail(ec, "write");

        // Receive the HTTP response
        http::async_read(stream_, buffer_, res_,
            beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));

    }

    void
        on_read(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        std::cout << "bytes Received: " << bytes_transferred << '\n';
        if (ec)
            return fail(ec, "read");

        // Write the message to standard out
        std::cout << res_ << '\n';

        // Gracefully close the socket
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes so don't bother reporting it.
        if (ec && ec != beast::errc::not_connected)
            return fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }

};

//------------------------------------------------------------------------------

int main()
{
    // Check command line arguments.
    // if (argc != 4 && argc != 5)
    // {
    //     std::cerr <<
    //         "Usage: http-client-async <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
    //         "Example:\n" <<
    //         "    http-client-async www.example.com 80 /\n" <<
    //         "    http-client-async www.example.com 80 / 1.0\n";
    //     return EXIT_FAILURE;
    // }
    auto const host = "0.0.0.0";
    auto const port = "8080";
    auto const target = "/login";
    int version = 11;

    // The io_context is required for all I/O
    net::io_context ioc;

    // Launch the asynchronous operation
    std::make_shared<session>(ioc)->run(host, port, target, version);

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return EXIT_SUCCESS;
}