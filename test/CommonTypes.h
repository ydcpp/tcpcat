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

#include "tcpcat.h"

#include <iostream>
#include <string>

class ServerHandler : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Client connected: " << session->RemoteEndpoint().address().to_string() << " : " << session->RemoteEndpoint().port()
                  << '\n';
    }

    void OnReceived(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::cout << "Message received from client: " << std::string(buf.begin(), buf.begin() + bytes) << '\n';
    }

    void OnSent(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::cout << "Message sent to client: " << std::string(buf.begin(), buf.begin() + bytes) << '\n';
    }

    void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Client disconnected: " << session->RemoteEndpoint().address().to_string() << " : " << session->RemoteEndpoint().port()
                  << '\n';
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::cout << "Error: " << err.message() << '\n';
    }
};

class ClientHandler : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Connected to server: " << session->RemoteEndpoint().address().to_string() << " : " << session->RemoteEndpoint().port()
                  << '\n';
    }

    void OnReceived(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::cout << "Message received from server: " << std::string(buf.begin(), buf.begin() + bytes) << '\n';
    }

    void OnSent(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::cout << "Message sent to server: " << std::string(buf.begin(), buf.begin() + bytes) << '\n';
    }

    void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Disconnected from server: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::cout << "Error: " << err.message() << '\n';
    }
};
