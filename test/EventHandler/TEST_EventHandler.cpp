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

#include <thread>
#include <mutex>

#include <gtest/gtest.h>

#include "tcpcat.h"

namespace
{
const std::string g_ClientToServer("EHandler test msg from client");
const std::string g_ServerToClient("EHandler test msg from server");
const uint16_t g_ServerPort = 55667;
std::recursive_mutex g_mtx;

class EventHandlerServerTest : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::lock_guard<std::recursive_mutex> lock(g_mtx);
        std::cout << "SessionID [" << session->GetId() << "] OnConnected: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
        const std::string clientIp = session->RemoteEndpoint().address().to_string();
        EXPECT_EQ(clientIp, "127.0.0.1");
    }

    void OnReceived(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::lock_guard<std::recursive_mutex> lock(g_mtx);
        std::cout << "SessionID [" << session->GetId() << "] OnReceived: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
        EXPECT_EQ(g_ClientToServer, std::string(buf.begin(), buf.begin() + bytes));
        std::vector<unsigned char> msg(g_ServerToClient.begin(), g_ServerToClient.end());
        session->Send(msg, 0, msg.size());
    }

    void OnSent(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        std::lock_guard<std::recursive_mutex> lock(g_mtx);
        std::cout << "SessionID [" << session->GetId() << "] OnSent: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
        EXPECT_EQ(g_ServerToClient, std::string(buf.begin(), buf.begin() + bytes));
    }

    void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::lock_guard<std::recursive_mutex> lock(g_mtx);
        std::cout << "SessionID [" << session->GetId() << "] OnDisconnected: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
        const std::string clientIp = session->RemoteEndpoint().address().to_string();
        EXPECT_EQ(clientIp, "127.0.0.1");
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::lock_guard<std::recursive_mutex> lock(g_mtx);
        std::cout << "SessionID [" << session->GetId() << "] OnError: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
        std::cout << "Error: " << err.message() << '\n';
    }
};

class EventHandlerClientTest : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        const std::string serverIp = session->RemoteEndpoint().address().to_string();
        const uint16_t serverPort = session->RemoteEndpoint().port();
        EXPECT_EQ(serverIp, "127.0.0.1");
        EXPECT_EQ(serverPort, g_ServerPort);
    }

    void OnReceived(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        EXPECT_EQ(g_ServerToClient, std::string(buf.begin(), buf.begin() + bytes));
        session->Close();
    }

    void OnSent(std::shared_ptr<tcpcat::TcpSession> session, const std::vector<unsigned char> &buf, size_t bytes) override
    {
        EXPECT_EQ(g_ClientToServer, std::string(buf.begin(), buf.begin() + bytes));
    }

    void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        const std::string serverIp = session->RemoteEndpoint().address().to_string();
        const uint16_t serverPort = session->RemoteEndpoint().port();
        EXPECT_EQ(serverIp, "127.0.0.1");
        EXPECT_EQ(serverPort, g_ServerPort);
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::cout << "Error: " << err.message() << '\n';
    }
};

};

TEST(EHandler, TEST_001)
{
    tcpcat::TcpServer server("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerServerTest>(), 256, 0);
    std::thread serverThr([&server]() -> void { server.Start(); });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    tcpcat::TcpClient client1("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client1.Connect();
    if (client1.IsConnected()) {
        client1.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    tcpcat::TcpClient client2("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client2.Connect();
    if (client2.IsConnected()) {
        client2.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    tcpcat::TcpClient client3("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client3.Connect();
    if (client3.IsConnected()) {
        client3.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    tcpcat::TcpClient client4("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client4.Connect();
    if (client4.IsConnected()) {
        client4.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    serverThr.join();
}

TEST(EHandler, TEST_002)
{
    tcpcat::TcpServer server("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerServerTest>(), 256, 0);
    server.StartNonBlocking();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    tcpcat::TcpClient client1("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client1.Connect();
    if (client1.IsConnected()) {
        client1.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    tcpcat::TcpClient client2("127.0.0.1", g_ServerPort, std::make_shared<EventHandlerClientTest>());
    client2.Connect();
    if (client2.IsConnected()) {
        client2.Send(std::vector<unsigned char>(g_ClientToServer.begin(), g_ClientToServer.end()));
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}
