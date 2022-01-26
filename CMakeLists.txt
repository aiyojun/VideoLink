cmake_minimum_required(VERSION 3.10.0)

project(VideoLink)

set(CMAKE_CXX_STANDARD 11)

set(SRC src/server.cpp src/helper.cpp)

add_executable(VideoLinkServer ${SRC})

target_link_libraries(VideoLinkServer X11 vncserver)
