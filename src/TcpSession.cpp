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

#include "tcpcat/base/TcpSession.h"

#include <asio.hpp>

#include "tcpcat/base/EventHandler.h"

namespace tcpcat
{

TcpSession::TcpSession(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<EventHandler> eventHandler, size_t buffSize) :
    socket_(socket),
    eventHandler_(eventHandler),
    readBuffer_(buffSize == 0 ? 256 : buffSize)
{
}

void TcpSession::OnConnected()
{
    remoteEndpoint_ = socket_->remote_endpoint();
    id_ = std::hash<std::string>()(remoteEndpoint_.address().to_string() + std::to_string(remoteEndpoint_.port()));
    eventHandler_->OnConnected(shared_from_this());
}

void TcpSession::OnError(const asio::error_code &err)
{
    eventHandler_->OnError(shared_from_this(), err);
}

size_t TcpSession::GetId() const
{
    return id_;
}

asio::ip::tcp::endpoint TcpSession::RemoteEndpoint() const
{
    return remoteEndpoint_;
}

void TcpSession::Close()
{
    if (socket_->is_open()) {
        socket_->shutdown(asio::ip::tcp::socket::shutdown_both);
        asio::error_code err;
        socket_->close(err);
        auto sharedThis = shared_from_this();
        err ? eventHandler_->OnError(sharedThis, err) : eventHandler_->OnDisconnected(sharedThis);
    }
}

size_t TcpSession::Send(const std::vector<unsigned char> &buffer, size_t offset, size_t size)
{
    auto sharedThis = shared_from_this();
    if (offset + size > buffer.size()) {
        const auto err = asio::error::make_error_code(asio::error::invalid_argument);
        eventHandler_->OnError(sharedThis, err);
        return 0;
    }

    asio::error_code err;
    const auto sentBytes = asio::write(*socket_, asio::buffer(buffer.data(), offset + size), err);
    sentBytes > 0 ? eventHandler_->OnSent(sharedThis, buffer, sentBytes) : eventHandler_->OnError(sharedThis, err);

    return sentBytes;
}

void TcpSession::SendAsync(const std::vector<unsigned char> &buffer, size_t offset, size_t size)
{
    auto sharedThis = shared_from_this();
    if (offset + size > buffer.size()) {
        const auto err = asio::error::make_error_code(asio::error::invalid_argument);
        eventHandler_->OnError(sharedThis, err);
        return;
    }

    asio::async_write(*socket_, asio::buffer(buffer.data() + offset, size),
                      [sharedThis, buffer](const asio::error_code &err, size_t bytes) {
                          if (err) {
                              sharedThis->eventHandler_->OnError(sharedThis, err);
                          } else {
                              sharedThis->eventHandler_->OnSent(sharedThis, buffer, bytes);
                          }
                      });
}

void TcpSession::Listen()
{
    if (!socket_->is_open()) {
        return;
    }
    auto sharedThis = shared_from_this();
    socket_->async_read_some(asio::buffer(readBuffer_.data(), readBuffer_.size()),
                             [sharedThis](const asio::error_code &err, size_t bytes) -> void {
                                 if (err) {
                                     if (err == asio::error::eof) {
                                         sharedThis->Close();
                                     } else if (err != asio::error::operation_aborted) {
                                         sharedThis->eventHandler_->OnError(sharedThis, err);
                                     }
                                     return;
                                 }

                                 sharedThis->eventHandler_->OnReceived(sharedThis, sharedThis->readBuffer_, bytes);
                                 sharedThis->Listen();
                             });
}

}
