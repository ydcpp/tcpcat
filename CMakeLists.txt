cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_VER 1.0.0)
set(PROJECT_NAME "tcpcat")
project(${PROJECT_NAME} LANGUAGES CXX
                        VERSION ${PROJECT_VER}
                        DESCRIPTION "C++ TCP Server and Client library.")

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
                                              ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
                                              ${CMAKE_CURRENT_SOURCE_DIR}/include/*.ipp)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} ${HDR_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "include" "include/asio")

option(ENABLE_TESTS "Enable unit tests." OFF)
if(${ENABLE_TESTS})
    message("${PROJECT_NAME}: Unit tests enabled.")
    enable_testing()
    add_subdirectory(test)
endif()

install(TARGETS tcpcat)
