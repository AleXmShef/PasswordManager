cmake_minimum_required(VERSION 2.8)

Project(PasswordManager CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../Bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/../Bin/)

link_directories("${CMAKE_BINARY_DIR}/../Bin")

add_subdirectory(PasswordManager)
add_subdirectory(CryptDLL)
