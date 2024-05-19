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
#include <atomic>

#include <gtest/gtest.h>

#include "tcpcat.h"
#include "CommonTypes.h"

TEST(Client, TEST_001)
{
    tcpcat::TcpClient client("45.79.112.203", 4242, std::make_shared<ClientHandler>());
    bool connected;
    ASSERT_NO_THROW(connected = client.Connect());
    if (connected) {
        ASSERT_NO_THROW(client.Disconnect());
    }
}

TEST(Client, TEST_002)
{
    tcpcat::TcpClient client("tcpbin.com", 4242, std::make_shared<ClientHandler>());
    bool connected;
    ASSERT_NO_THROW(connected = client.Connect());
    if (connected) {
        ASSERT_NO_THROW(client.Disconnect());
    }
}

class Test003Handler : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Test003Handler connected.\n";
        std::cout << "Server: " << session->RemoteEndpoint().address().to_string() << " : " << session->RemoteEndpoint().port()
                  << '\n';
        session->Close();
    }

    void OnDisconnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Test003Handler disconnected.\n";
        std::cout << "Server: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::cout << "Error: " << err.message() << '\n';
    }
};

TEST(Client, TEST_003)
{
    tcpcat::TcpClient client("tcpbin.com", 4242, std::make_shared<Test003Handler>());
    ASSERT_NO_THROW(client.ConnectAsync());
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Client, TEST_004)
{
    tcpcat::TcpClient client("tcpbin.com", 4242, std::make_shared<ClientHandler>());
    ASSERT_NO_THROW(client.Connect());
    if (client.IsConnected()) {
        const std::string msg = "hello tcpcat";
        client.Send(std::vector<unsigned char>(msg.begin(), msg.end()));
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
