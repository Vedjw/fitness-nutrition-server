#include "handle_request_functions.h"
#include "user_service.h"
#include "Error.h"
#include "Random.h"
#include "Ringbuf.h"
#include "calories.h"

extern RingBuf g_ring_buf;

// Error Responses
http::response<http::string_body> bad_request(
    const std::string_view& why,
    http::request<http::string_body>&& req
) {
    http::response<http::string_body> res{ http::status::bad_request, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

http::response<http::string_body> server_error(
    const std::string_view& what,
    http::request<http::string_body>&& req
) {
    http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
}

http::response<http::string_body> not_found(
    const std::string_view& what,
    http::request<http::string_body>&& req
) {
    http::response<http::string_body> res{ http::status::not_found, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "File '" + std::string(what) + "' not found";
    res.prepare_payload();
    return res;
}

beast::string_view mime_type(beast::string_view path) {
    using beast::iequals;
    auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if (pos == beast::string_view::npos)
                return beast::string_view{};
            return path.substr(pos);
        }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php"))  return "text/html";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".txt"))  return "text/plain";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml"))  return "application/xml";
    if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))  return "video/x-flv";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".jpe"))  return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    if (iequals(ext, ".gif"))  return "image/gif";
    if (iequals(ext, ".bmp"))  return "image/bmp";
    if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif"))  return "image/tiff";
    if (iequals(ext, ".svg"))  return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Handlers
http::response<http::string_body> handle_signup(
    http::request<http::string_body>&& req
) {
    //std::cout << req << '\n';//
    if (req[http::field::content_type] != "text/plain") {
        return bad_request("Unknown content type", std::move(req));
    }

    std::string session_cookie = std::string(req[http::field::set_cookie]);

    try {
        Error return_err = make_user(session_cookie, req.body());
        if (!return_err.success) {
            return bad_request(return_err.message, std::move(req));
        }
    }
    catch (const char* err) {
        return server_error(err, std::move(req));
    }


// Success response
    http::response<http::string_body> res{ http::status::ok, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.body() = "New User created";
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return res;

}

http::response<http::string_body> handle_login(
    http::request<http::string_body>&& req
) {
    if (req[http::field::content_type] != "text/plain") {
        return bad_request("Unknown content type", std::move(req));
    }

    std::string session_cookie = std::string(req[http::field::cookie]);
    std::cout << session_cookie << '\n'; //
    if (session_cookie == "") {
        return bad_request("No cookie info", std::move(req));
    }

    try {
        Error return_err = auth_user(session_cookie, req.body());
        if (!return_err.success) {
            return bad_request(return_err.message, std::move(req));
        }
    }
    catch (const char* err) {
        return server_error(err, std::move(req));
    }

    http::response<http::string_body> res{ http::status::ok, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.body() = "Valid User";
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return res;
}

http::response<http::string_body> handle_tracker(
    http::request<http::string_body>&& req
) {
    if (req[http::field::content_type] != "text/plain") {
        return bad_request("Unknown content type", std::move(req));
    }

    std::string cookie = std::string(req[http::field::cookie]);
    if (cookie == "") {
        return bad_request("No cookie info", std::move(req));
    }

    // auto user = ring_buf.find(session_id);
    // user->print();

    if (req.target() == "/api/tracker") {
        std::cout << "from here" << '\n';
        Error return_err = user_fitness_tracking(cookie, req.body());
        if (!return_err.success) {
            return bad_request(return_err.message, std::move(req));
        }
    }
    http::response<http::string_body> res{ http::status::ok, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.body() = "Info Updated";
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return res;
}


// Handle POST
http::response<http::string_body> handle_post(
    http::request<http::string_body>&& req
) {
    auto target = req.target();
    std::cout << "target path: " << target << '\n';

    if (req.body().empty()) {
        return bad_request("Empty body", std::move(req));
    }

    if (target == "/api/profile") {
        return handle_signup(std::move(req));
    }

    if (target == "/api/login") {
        return handle_login(std::move(req));
    }

    if (target == "/api/tracker") {
        return handle_tracker(std::move(req));
    }

    return server_error("Unable to handle post req", std::move(req));
}


http::response<http::string_body> handle_get_data(
    http::request<http::string_body>&& req
) {
    using json = nlohmann::json;
    std::string cookie_str = std::string(req[http::field::cookie]);
    if (cookie_str == "") {
        return bad_request("No cookie info", std::move(req));
    }

    if (req.target() != "/api/results") {
        return bad_request("Unknown target url", std::move(req));
    }

    auto user = g_ring_buf.find(get_cookie(cookie_str));
    if (!user) {
        return not_found("User not in cache", std::move(req));
    }

    json data;

    data = {
        {"fitnessScore", 90},
        {"BMR", calBMR(user->getWeight(), user->getHeight(), user->getAge(), user->getGender())},
        {"BMI", calBMI(user->getWeight(), user->getHeight())},
        {"caloriesConsumed", user->getCalConsumed()},
        {"caloriesBurned", user->getCalBurned()}
    };

    std::string data_str = data.dump();

    http::response<http::string_body> res{ http::status::ok, req.version() };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.body() = data_str;
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return res;
}


std::string generate_cookie() {
    auto ssid = Random::get<uint16_t>(0, 10000);
    return std::to_string(ssid);
}

std::string get_cookie(std::string& cookie_string) {
    auto pos = cookie_string.rfind("=");
    if (pos == std::string::npos) {
        return std::string{};
    }
    ++pos;
    std::string ret_str = cookie_string.substr(pos);
    return ret_str;
}
