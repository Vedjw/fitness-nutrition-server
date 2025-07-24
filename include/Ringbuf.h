#pragma once

#include <iostream>
#include <memory>
#include <array>
#include <string>
#include <string_view>
#include "User.h"

const size_t g_buf_size = 10;

class RingBuf {
    int m_cur_index{ 0 };
    std::array<std::pair<std::string, std::shared_ptr<User>>, g_buf_size> m_ringbuf;

public:
    std::shared_ptr<User> find(std::string_view session_id) {

        for (int i{ 0 }; i < m_cur_index; ++i) {
            if (m_ringbuf[i].first == session_id) {
                auto x = m_ringbuf[i].second;
                return x;
            }
        }

        return nullptr;
    }

    void cache_user(std::string session_id, std::shared_ptr<User> user) {

        m_ringbuf[m_cur_index].first = session_id;
        m_ringbuf[m_cur_index].second = std::move(user);
        m_ringbuf[m_cur_index].second->print();//
        if (m_cur_index == g_buf_size - 1) {
            m_cur_index = 0;
            return;
        }
        m_cur_index++;
    }
};
