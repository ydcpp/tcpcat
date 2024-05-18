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

#include "base/TcpServer.h"

#include <iostream>

namespace tcpcat
{

TcpServer::~TcpServer()
{
    for (const auto &[id, session] : sessions_) {
        session->Close();
    }
    sessions_.clear();
    ctx_.stop();
    running_ = false;
    for (auto &worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

TcpServer::TcpServer(const std::string &ip, uint16_t port, std::shared_ptr<EventHandler> handler, size_t bufferSize, uint8_t threadCount) :
    ip_(ip),
    port_(port),
    ctx_(),
    ctxWork_(ctx_),
    endpoint_({}, port),
    acceptor_(ctx_, endpoint_),
    threadPoolSize_(threadCount == 0 ? 1 : threadCount),
    handler_(handler),
    buffSize_(bufferSize)
{
}

void TcpServer::OnStarted()
{
    std::cout << "TcpServer started listening on port: " << port_ << '\n';
}

void TcpServer::OnStopped()
{
    std::cout << "TcpServer stopped.\n";
}

void TcpServer::Start()
{
    if (!running_) {
        ctx_.restart();
        for (uint16_t i = 0; i < threadPoolSize_; i++) {
            workers_.emplace_back([this]() { ctx_.run(); });
        }
        running_ = true;
        OnStarted();
        while (running_) {
            auto session = std::make_shared<TcpSession>(ctx_, handler_, buffSize_);
            asio::error_code err;
            acceptor_.accept(*session->Socket(), err);
            if (!running_) {
                return;
            }
            if (err) {
                session->OnError(err);
                session.reset();
            } else {
                sessions_[session->GetId()] = session;
                session->OnConnected();
                session->Listen();
            }
        }
    }
}

void TcpServer::StartNonBlocking()
{
    if (!running_) {
        ctx_.restart();
        for (uint16_t i = 0; i < threadPoolSize_; i++) {
            workers_.emplace_back([this]() { ctx_.run(); });
        }
        running_ = true;
        auto session = std::make_shared<TcpSession>(ctx_, handler_, buffSize_);
        acceptor_.async_accept(*session->Socket(), bind(&TcpServer::HandleAccept, this, session, asio::placeholders::error));
        OnStarted();
    }
}

void TcpServer::Stop()
{
    if (running_) {
        ctx_.stop();
        for (auto &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers_.clear();
        running_ = false;
        acceptor_.close();
        OnStopped();
    }
}

bool TcpServer::IsRunning() const
{
    return running_;
}

std::string TcpServer::GetIp() const
{
    return ip_;
}

uint16_t TcpServer::GetPort() const
{
    return port_;
}

std::shared_ptr<TcpSession> TcpServer::GetSession(size_t id)
{
    const auto it = sessions_.find(id);
    return it == sessions_.end() ? nullptr : it->second;
}

void TcpServer::HandleAccept(std::shared_ptr<TcpSession> session, const asio::error_code &err)
{
    if (!err) {
        session->OnConnected();
        session->Listen();
        sessions_[session->GetId()] = session;
        session = std::make_shared<TcpSession>(ctx_, handler_, buffSize_);
        acceptor_.async_accept(*session->Socket(), std::bind(&TcpServer::HandleAccept, this, session, asio::placeholders::error));
    } else {
        session->OnError(err);
        session.reset();
    }
}

}