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

#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <memory>

#include <asio.hpp>

#include "TcpSession.h"
#include "EventHandler.h"

namespace tcpcat
{

class TcpClient
{
public:
    ~TcpClient();
    TcpClient() = delete;
    TcpClient(const TcpClient &) = delete;
    TcpClient &operator=(const TcpClient &) = delete;

    /// @brief Constructor for creating a TcpClient instance.
    /// @param host Host name, can be IP address or domain name.
    /// @param port Port number.
    /// @param handler Event handler object for callbacks.
    /// @param bufferSize I/O buffer size.
    TcpClient(const std::string &host, uint16_t port, std::shared_ptr<EventHandler> handler, size_t bufferSize = 256);

    /// @brief Starts connection with host. EventHandler->OnConnected will be called on success.
    /// @return Returns true on success, false otherwise.
    bool Connect();

    /// @brief Starts connection with non-blocking operation. EventHandler->OnConnected will be called on success.
    void ConnectAsync();

    /// @brief Disconnects from host. EventHandler->OnDisconnected will be called on success.
    void Disconnect();

    /// @brief Sends data to host. EventHandler->OnSent will be called on success.
    /// @param data Buffer that holds the data to be sent.
    /// @return Returns the number of bytes sent.
    size_t Send(const std::vector<unsigned char> &data);

    /// @brief Sends data to host. EventHandler->OnSent will be called on success.
    /// @param data Buffer that holds the data to be sent.
    /// @param offset Offset value from the beginning of `data`.
    /// @param size Count of bytes to be sent starting from `offset`.
    /// @return Returns the number of bytes sent.
    size_t Send(const std::vector<unsigned char> &data, size_t offset, size_t size);

    /// @brief Sends data to host with non-blocking operation. EventHandler->OnSent will be called on success.
    /// @param data Buffer that holds the data to be sent.
    void SendAsync(const std::vector<unsigned char> &data);

    /// @brief Sends data to host with non-blocking operation. EventHandler->OnSent will be called on success.
    /// @param data Buffer that holds the data to be sent.
    /// @param size Count of bytes to be sent.
    void SendAsync(const std::vector<unsigned char> &data, size_t size);

    /// @brief Gets host IP or domain name.
    /// @return Returns host IP or domain name.
    std::string GetHost() const;

    /// @brief Gets host port number.
    /// @return Returns host port number.
    uint16_t GetPort() const;

    /// @brief Gets client connection state.
    /// @return Returns true if connected, false otherwise.
    bool IsConnected() const;

private:
    std::string host_;
    uint16_t port_;
    std::atomic<bool> connected_;
    asio::io_context ctx_;
    asio::io_context::work ctxWork_;
    std::shared_ptr<asio::ip::tcp::socket> socket_;
    asio::ip::tcp::endpoint endpoint_;
    std::thread ctxRunner_;
    std::shared_ptr<TcpSession> session_;

private:
    void HandleConnect(const asio::error_code &err);
};

}
