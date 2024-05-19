/*
* MIT License
* Copyright (c) 2024 ydcpp
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <vector>
#include <memory>

#include "tcpcat/deps/asio.hpp"

#include "TcpSession.h"

namespace tcpcat
{

class EventHandler
{
public:
    virtual ~EventHandler() = default;

    /// @brief Gets called when connected to peer.
    /// @param session Session handle object.
    virtual void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) {}

    /// @brief Gets called when disconnected from peer.
    /// @param session Session handle object.
    virtual void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) {}

    /// @brief Gets called when received a message from peer.
    /// @param session Session handle object.
    /// @param buf Contains the received bytes.
    /// @param bytes Received byte count.
    virtual void OnReceived(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) {}

    /// @brief Gets called when sent a message to peer.
    /// @param session Session handle object.
    /// @param buf Contains the sent bytes.
    /// @param bytes Sent byte count.
    virtual void OnSent(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) {}

    /// @brief Gets called when an error occurs.
    /// @param session Session handle object.
    /// @param err Error code object which holds error information.
    virtual void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) {}
};

}
