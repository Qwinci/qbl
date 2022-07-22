cmake_minimum_required(VERSION 3.21)
include(kernel.cmake)
project(BasicKernel)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.hpp)

add_executable(basicKernel ${SRC} ${HEADERS})