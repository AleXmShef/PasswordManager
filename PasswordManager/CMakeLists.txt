cmake_minimum_required(VERSION 3.10)
project(PasswordManagerUI)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "K:/Qt/5.14.1/mingw73_64")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)

include_directories(../CryptDLL/include)

add_executable(PasswordManagerUI source/main.cpp qt/qml.qrc)

target_compile_definitions(PasswordManagerUI
        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(PasswordManagerUI Qt5::Core)
target_link_libraries(PasswordManagerUI Qt5::Widgets)
target_link_libraries(PasswordManagerUI Qt5::Quick)
target_link_libraries(PasswordManagerUI CryptDLL.dll)