#include <iostream>
#include <string>

#include "tcpcat.h"

class MyClientHandler : public tcpcat::EventHandler
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

int main()
{
    auto handler = std::make_shared<MyClientHandler>();
    tcpcat::TcpClient client("tcpbin.com", 4242, handler);

    // non-blocking connect, handler->OnConnected will be triggered on success, handler->OnError will be triggered on fail.
    // client.ConnectAsync();

    // blocking connect, check return value or client.IsConnected() to verify connection.
    const bool connResult = client.Connect();
    if (!connResult) {
        std::cout << "Failed to connect to server.\n";
        return -1;
    }

    // send data to server
    // client.Send or client.SendAsync
    client.SendAsync({ 65, 66, 67, 68 });    // send bytes

    const std::string msg("hello world");
    client.Send(std::vector<unsigned char>(msg.begin(), msg.end()));    // send string

    client.Disconnect();    // handler->OnDisconnected will be triggered.

    return 0;
}
