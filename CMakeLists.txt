cmake_minimum_required(VERSION 3.3)
project(Net_Server_Basic)

set(Net_Server_Basic_VERSION_MAJOR 1)
set(Net_Server_Basic_VERSION_MINOR 0)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES src/main.cpp src/core.cpp src/net.cpp src/core/CException.cpp src/net/CServer.cpp src/core/CError.cpp src/net/CSocket.cpp)
add_executable(Net_Server_Basic ${SOURCE_FILES})

target_link_libraries(Net_Server_Basic "${CMAKE_CURRENT_SOURCE_DIR}/lib/libws2_32.a")