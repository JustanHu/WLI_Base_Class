cmake_minimum_required(VERSION 3.19)
project(WLI_Base_Class)

set(CMAKE_CXX_STANDARD 14)

add_executable(WLI_Base_Class main.cpp brukerWLI/brukerWLI.cpp brukerWLI/brukerWLI.h)
target_link_libraries(WLI_Base_Class ws2_32)