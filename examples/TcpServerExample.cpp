#include <iostream>
#include <string>

#include "tcpcat.h"

class MyServerHandler : public tcpcat::EventHandler
{
    void OnConnected(std::shared_ptr<tcpcat::TcpSession> session) override
    {
        std::cout << "Client connected: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
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
        std::cout << "Client disconnected: " << session->RemoteEndpoint().address().to_string() << " : "
                  << session->RemoteEndpoint().port() << '\n';
    }

    void OnError(std::shared_ptr<tcpcat::TcpSession> session, const asio::error_code &err) override
    {
        std::cout << "Error: " << err.message() << '\n';
    }
};

int main()
{
    auto handler = std::make_shared<MyServerHandler>();
    const size_t bufferSize = 1024;
    const uint8_t threadCount = 4;

    tcpcat::TcpServer server("127.0.0.1", 3001, handler, bufferSize, threadCount);

    // non-blocking start, ensure the server object lifetime is handled when using this.
    // server.StartNonBlocking();

    // blocking start, waits for new connections to accept.
    server.Start();

    return 0;
}
