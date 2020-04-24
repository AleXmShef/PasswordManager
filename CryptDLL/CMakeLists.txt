cmake_minimum_required(VERSION 3.10)
project(CryptDLL)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
link_directories(lib)

add_library(CryptDLL SHARED source/PasswordManager.cpp)

target_link_libraries(CryptDLL
        crypto)