cmake_minimum_required(VERSION 3.15)

set(PROJECT_VER 1.0.5)
set(PROJECT_NAME "ydcpp-tcpcat")
project(${PROJECT_NAME} LANGUAGES CXX
                        VERSION ${PROJECT_VER}
                        DESCRIPTION "C++ TCP Server and Client library.")

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
                                              ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
                                              ${CMAKE_CURRENT_SOURCE_DIR}/include/*.ipp)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} ${HDR_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(asio CONFIG)
if(asio_FOUND)
    message("tcpcat: Using system installed ASIO headers.")
    target_link_libraries(${PROJECT_NAME} asio::asio)
else()
    message("tcpcat: Using project local ASIO headers in 'deps/asio'.")
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/asio)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deps/asio/ DESTINATION include)
endif()

option(ENABLE_TESTS "Enable unit tests." OFF)
if(${ENABLE_TESTS})
    message("${PROJECT_NAME}: Unit tests enabled.")
    enable_testing()
    add_subdirectory(test)
endif()

install (TARGETS ${PROJECT_NAME}
         PUBLIC_HEADER DESTINATION include
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/tcpcat DESTINATION include)
