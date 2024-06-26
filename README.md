# tcpcat
An easy to use, cross-platform C++ TCP Server and Client library with ASIO.

## Examples
### TCP Server Example
~~~cpp
#include <iostream>
#include <string>

#include "tcpcat/tcpcat.h"

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

    server.Start();

    return 0;
}
~~~

### TCP Client Example
~~~cpp
#include <iostream>
#include <string>

#include "tcpcat/tcpcat.h"

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

    client.Connect();
    if (!client.IsConnected()) {
        std::cout << "Failed to connect to server.\n";
        return -1;
    }


    client.SendAsync({ 65, 66, 67, 68 });    // send bytes

    const std::string msg("hello tcpcat");
    client.Send(std::vector<unsigned char>(msg.begin(), msg.end()));    // send string

    client.Disconnect();

    return 0;
}
~~~

## Build with CMake (Windows/Linux/MacOS)
### Configure
~~~shell
cmake -S . -B build
~~~

### Build
~~~shell
cmake --build build --config Release
~~~

**Note:** If you want to build with unit tests, add `-DENABLE_TESTS=ON` to CMake configure command.

### Install
~~~shell
cmake --install build
~~~

## Build with Conan (Windows/Linux/MacOS)
### Creating tcpcat package in conan local cache
~~~shell
conan create .
~~~
Validate `ydcpp-tcpcat` package exists by using following command to list all installed packages in local conan cache:
```
conan list "*"
```
### Using tcpcat package in  your project
1- Create `conanfile.py` or `conanfile.txt` file in the root of your project.

**conanfile.txt**
```
[requires]
ydcpp-tcpcat/[^1]

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
```

**conanfile.py**
```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("ydcpp-tcpcat/[^1]")

    def layout(self):
        cmake_layout(self)
```

2- Use following command (in the root directory of project) to install all dependencies that are specified in the `conanfile`:
```
conan install . --build=missing
```

3- Edit `CMakeLists.txt` of your project to link against libraries. In this case, we link to `ydcpp-tcpcat` library.

Add these lines after declaring the target in CMakeLists.

**CMakeLists.txt**
```cmake
find_package(ydcpp-tcpcat)
target_link_libraries(<your_target> ydcpp-tcpcat)
```

Make sure to edit `<your_target>` with the actual target name.

4- Use tcpcat header:
```cpp
#include <tcpcat/tcpcat.h>
```

