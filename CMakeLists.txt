cmake_minimum_required(VERSION 3.15)
project(automan_jvm)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "*.h")
file(GLOB HEADERS "*.cpp")

file(GLOB NATIVE_SOURCES "native/*.h")
file(GLOB NATIVE_HEADERS "native/*.cpp")

file(GLOB RUNTIME_SOURCES "runtime/*.h")
file(GLOB RUNTIME_HEADERS "runtime/*.cpp")

file(GLOB UTILS_SOURCES "utils/*.h")
file(GLOB UTILS_HEADERS "utils/*.cpp")

add_library(automan_jvm ${SOURCES} ${HEADERS} ${NATIVE_SOURCES} ${NATIVE_HEADERS} ${RUNTIME_SOURCES} ${RUNTIME_HEADERS} ${UTILS_SOURCES} ${UTILS_HEADERS})
