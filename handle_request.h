#pragma once

#include "server_constants.h"
#include "handle_request_functions.h"
#include "handle_get.h"

template <class Body, class Allocater>
http::message_generator handle_request(
    http::request<Body, http::basic_fields<Allocater>>&& req
) {

    if (req.target().empty()) {
        return bad_request("Empty Target", std::move(req));
    }

    switch (req.method()) {
    case http::verb::get:
        return handle_get(std::move(req));
    case http::verb::post:
        return handle_post(std::move(req));
    default:
        return bad_request("Unknown HTTP-method", std::move(req));
    }
}