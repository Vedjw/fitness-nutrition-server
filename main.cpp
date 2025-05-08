#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include "calories.h"
#include "User.h"
#include "constants.h"
#include "Listener.h"
#include "Session.h" 
#include "Store.h"
#include "Ringbuf.h"

Store g_DB; // cannot use the keyword extern here or else the compiler will think this is an forward declaration.
RingBuf g_ring_buf;

int main() {

    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi("8080"));
    auto const threads = std::max<int>(1, std::atoi("2"));

    net::io_context ioc{ threads };

    // Create and launch a listening port
    std::make_shared<Listener>(
        ioc,
        tcp::endpoint{ address, port })->run();

    std::cout << "Server Running" << '\n';

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
    ioc.run();

    return EXIT_SUCCESS;

    return 0;
}