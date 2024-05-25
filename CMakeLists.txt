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
target_include_directories(${PROJECT_NAME} PUBLIC "include")

option(ENABLE_TESTS "Enable unit tests." OFF)
if(${ENABLE_TESTS})
    message("${PROJECT_NAME}: Unit tests enabled.")
    enable_testing()
    add_subdirectory(test)
endif()

install(TARGETS tcpcat)

# cppcheck static code analysis
find_program(CPP_CHECK cppcheck)
set(CPPCHECK_TEMPLATE "cppcheck: {file}:{line}: [{severity}]: {message}")

add_custom_target(
    cppcheck
    COMMAND ${CPP_CHECK}
    -j 4
    --platform=win64
    --enable=style,warning,performance,portability
    --language=c++
    --std=c++17
    --error-exitcode=1
    --max-configs=4
    --max-ctu-depth=1
    --template=${CPPCHECK_TEMPLATE}
    --suppress=*:*deps*
    -I ${CMAKE_SOURCE_DIR}/include
    --cppcheck-build-dir=${CMAKE_SOURCE_DIR}/build
    ${SRC_FILES}
)
