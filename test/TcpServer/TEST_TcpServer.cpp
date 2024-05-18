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

TEST(Server, TEST_001)
{
    tcpcat::TcpServer server("127.0.0.1", 55889, std::make_shared<ServerHandler>());
    std::thread t([&server](){
        ASSERT_NO_THROW(server.Start());
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(server.IsRunning());
    ASSERT_NO_THROW(server.Stop());
    EXPECT_FALSE(server.IsRunning());
    t.join();
}
TEST(Server, TEST_002)
{
    tcpcat::TcpServer server("127.0.0.1", 55889, std::make_shared<ServerHandler>());
    server.StartNonBlocking();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(server.IsRunning());
    ASSERT_NO_THROW(server.Stop());
    EXPECT_FALSE(server.IsRunning());
}

TEST(Server, TEST_003)
{
    const std::string ip = "1.2.3.4";
    const uint16_t port = 55889;
    tcpcat::TcpServer server(ip, port, std::make_shared<ServerHandler>());
    EXPECT_EQ(server.GetIp(), ip);
    EXPECT_EQ(server.GetPort(), port);
}
