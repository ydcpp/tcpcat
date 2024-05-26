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

#include "tcpcat/base/TcpClient.h"

namespace tcpcat
{

TcpClient::~TcpClient()
{
    ctx_.stop();
    if (connected_) {
        if (socket_->is_open()) {
            socket_->close();
        }
        connected_ = false;
    }

    if (ctxRunner_.joinable()) {
        ctxRunner_.join();
    }
}

TcpClient::TcpClient(const std::string &host, uint16_t port, std::shared_ptr<EventHandler> handler, size_t bufferSize) :
    host_(host),
    port_(port),
    connected_(false),
    ctx_(),
    ctxWork_(ctx_),
    socket_(std::make_shared<asio::ip::tcp::socket>(ctx_)),
    ctxRunner_(std::thread([this]() { ctx_.run(); })),
    session_(std::make_shared<TcpSession>(socket_, handler, bufferSize))
{
}

bool TcpClient::Connect()
{
    if (!connected_) {
        asio::error_code err;
        const auto resolverResult = asio::ip::tcp::resolver(ctx_).resolve(host_, std::to_string(port_), err);
        if (err) {
            session_->OnError(err);
            return false;
        }
        endpoint_ = resolverResult.begin()->endpoint();
        socket_->connect(endpoint_, err);
        HandleConnect(err);
    }

    return connected_;
}

void TcpClient::ConnectAsync()
{
    if (!connected_) {
        asio::error_code err;
        const auto resolverResult = asio::ip::tcp::resolver(ctx_).resolve(host_, std::to_string(port_), err);
        if (err) {
            session_->OnError(err);
            return;
        }
        endpoint_ = resolverResult.begin()->endpoint();
        socket_->async_connect(endpoint_, std::bind(&TcpClient::HandleConnect, this, asio::placeholders::error));
    }
}

void TcpClient::Disconnect()
{
    if (connected_) {
        connected_ = false;
        session_->Close();
    }
}

size_t TcpClient::Send(const std::vector<unsigned char> &data)
{
    return Send(data, 0, data.size());
}

size_t TcpClient::Send(const std::vector<unsigned char> &data, size_t offset, size_t size)
{
    if (!connected_) {
        return 0;
    }

    return session_->Send(data, offset, size);
}

void TcpClient::SendAsync(const std::vector<unsigned char> &data)
{
    session_->SendAsync(data, 0, data.size());
}

void TcpClient::SendAsync(const std::vector<unsigned char> &data, size_t size)
{
    session_->SendAsync(data, 0, size);
}

std::string TcpClient::GetHost() const
{
    return host_;
}

uint16_t TcpClient::GetPort() const
{
    return port_;
}

bool TcpClient::IsConnected() const
{
    return connected_;
}

void TcpClient::HandleConnect(const asio::error_code &err)
{
    if (err) {
        connected_ = false;
        session_->OnError(err);
    } else {
        connected_ = true;
        session_->OnConnected();
        session_->Listen();
    }
}

}
