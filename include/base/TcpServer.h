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
#include <memory>
#include <atomic>
#include <thread>
#include <unordered_map>

#include "asio/asio.hpp"

#include "TcpSession.h"
#include "EventHandler.h"

namespace tcpcat
{

class TcpServer
{
public:
    virtual ~TcpServer();
    TcpServer() = delete;
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(const TcpServer &) = delete;

    /// @brief Constructor for creating TcpServer instance.
    /// @param ip IP address.
    /// @param port Listen port number.
    /// @param handler Event handler object for callbacks.
    /// @param bufferSize I/O buffer size.
    /// @param threadCount Specifies how many threads server should run for handling connections.
    TcpServer(const std::string &ip, uint16_t port, std::shared_ptr<EventHandler> handler, size_t bufferSize = 256, uint8_t threadCount = 8);

    /// @brief Gets called when TCP server is started, writes started message on standard output.
    virtual void OnStarted();

    /// @brief Gets called when TCP server is stopped, writes stopped message on standard output.
    virtual void OnStopped();

    /// @brief Starts the server and blocks calling thread for accepting client connections.
    void Start();

    /// @brief Starts the server in non-blocking operation.
    void StartNonBlocking();

    /// @brief Stops the server.
    void Stop();

    /// @brief Gets running status of the server.
    /// @return Returns true if the server is running, false otherwise.
    bool IsRunning() const;

    /// @brief Gets IP address of the server.
    /// @return Returns IP address of the server.
    std::string GetIp() const;

    /// @brief Gets listening port number of the server.
    /// @return Returns listening port number.
    uint16_t GetPort() const;

    /// @brief Gets session by session ID.
    /// @param id Session ID.
    /// @return Returns related TcpSession if exists, nullptr otherwise.
    [[nodiscard]] std::shared_ptr<TcpSession> GetSession(size_t id);

private:
    std::atomic<bool> running_;
    asio::io_context ctx_;
    asio::io_context::work ctxWork_;
    asio::ip::tcp::endpoint endpoint_;
    asio::ip::tcp::acceptor acceptor_;
    uint8_t threadPoolSize_;
    std::vector<std::thread> workers_;
    std::string ip_;
    uint16_t port_;
    std::shared_ptr<EventHandler> handler_;
    size_t buffSize_;
    std::unordered_map<size_t, std::shared_ptr<TcpSession>> sessions_;

private:
    void HandleAccept(std::shared_ptr<TcpSession> session, const asio::error_code &err);
};

}
