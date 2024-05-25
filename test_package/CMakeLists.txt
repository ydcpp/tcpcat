cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

add_executable(example src/example.cpp)

find_package(tcpcat CONFIG REQUIRED)
target_link_libraries(example tcpcat)
