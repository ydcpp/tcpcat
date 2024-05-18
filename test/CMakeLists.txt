cmake_minimum_required(VERSION 3.15)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                                              ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(tcpcatTests)
target_sources(tcpcatTests PRIVATE ${SRC_FILES} ${HDR_FILES})
target_include_directories(tcpcatTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tcpcatTests tcpcat)
target_link_libraries(tcpcatTests gtest gtest_main)

add_test(NAME UT_tcpcat
         COMMAND tcpcatTests
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
