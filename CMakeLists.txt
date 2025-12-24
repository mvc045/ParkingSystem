cmake_minimum_required(VERSION 3.10)

project(Parking VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Подключаем uWebSockets
add_definitions(-DLIBUS_NO_SSL)
file(GLOB_RECURSE USOCKETS_SOURCES "libs/uSockets/src/*.c" "libs/uSockets/src/*.cpp")
add_library(uSockets STATIC ${USOCKETS_SOURCES})
target_include_directories(uSockets PUBLIC "libs/uSockets/src")

include_directories(libs/uWebSockets/src)

# Где искать заголовочные файлы (.hpp)
include_directories(include)

# (Это нужно, чтобы они появились в дереве Xcode)
file(GLOB HEADERS "include/*.hpp" "include/*.h")

file(GLOB CONFIGS "config.txt")

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Resources" FILES ${CONFIGS})

# Собираем список всех .cpp файлов из папки src
file(GLOB SOURCES "src/*.cpp")
add_executable(Parking ${SOURCES} ${HEADERS} ${CONFIGS})


# 2. ЭМУЛЯТОР RFID (C++ Tool)
file(GLOB RFID_TOOL_SOURCES "Emulators/RFIDTool/*.cpp")
source_group("Emulator Source" FILES ${RFID_TOOL_SOURCES})
add_executable(RfidTool ${RFID_TOOL_SOURCES})

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(Parking Threads::Threads sqlite3 uSockets ZLIB::ZLIB)

# ОТКЛЮЧИТЬ DTRACE
set_target_properties(Parking PROPERTIES XCODE_ATTRIBUTE_ENABLE_DTRACE "NO")

# В папке build
# 1. Запускаем сборку cmake ..
# 2. make
