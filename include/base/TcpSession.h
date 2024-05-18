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
#include <optional>

#include "asio.hpp"

namespace tcpcat
{

class EventHandler;

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
    ~TcpSession() = default;
    TcpSession() = delete;
    TcpSession(const TcpSession &) = delete;
    TcpSession &operator=(const TcpSession &) = delete;

    /// @brief Used by Server or Client classes, do not manually create TcpSession.
    TcpSession(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<EventHandler> eventHandler, size_t buffSize);

    /// @brief Sets RemoteEndpoint and calls EventHandler->OnConnected when connection is established with the peer.
    void OnConnected();

    /// @brief Calls EventHandler->OnError when an error occurs.
    /// @param err Error code object which holds error information.
    void OnError(const asio::error_code &err);

    /// @brief Closes TCP socket. Calls EventHandler->OnDisconnected on success, EventHandler->OnError otherwise.
    void Close();

    /// @brief Sends data to the peer. Calls EventHandler->OnSent on success, EventHandler->OnError otherwise.
    /// @param buffer Contains bytes to be sent.
    /// @param offset Offset value from the beginning of `buffer`.
    /// @param size Count of bytes to be sent starting from `offset`.
    /// @return Returns the number of bytes sent.
    size_t Send(const std::vector<unsigned char> &buffer, size_t offset, size_t size);

    /// @brief Sends data to the peer in non-blocking operation. Calls EventHandler->OnSent on success, EventHandler->OnError otherwise.
    /// @param buffer Contains bytes to be sent.
    /// @param offset Offset value from the beginning of `buffer`.
    /// @param size Count of bytes to be sent starting from `offset`.
    void SendAsync(const std::vector<unsigned char> &buffer, size_t offset, size_t size);

    /// @brief Starts listening for incoming data.
    void Listen();

    /// @brief Gets session ID.
    /// @return Returns session ID.
    size_t GetId() const;

    /// @brief Gets peer endpoint information.
    /// @return Returns peer endpoint information.
    asio::ip::tcp::endpoint RemoteEndpoint() const;

private:
    std::shared_ptr<asio::ip::tcp::socket> socket_;
    std::vector<unsigned char> readBuffer_;
    std::shared_ptr<EventHandler> eventHandler_;
    asio::ip::tcp::endpoint remoteEndpoint_;
    size_t id_;
};

}
