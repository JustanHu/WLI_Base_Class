cmake_minimum_required(VERSION 3.19)
project(WLI_Basic)

set(CMAKE_CXX_STANDARD 14)

add_executable(WLI_Basic main.cpp brukerWLI/brukerWLI.cpp brukerWLI/brukerWLI.h)
target_link_libraries(WLI_Basic ws2_32)