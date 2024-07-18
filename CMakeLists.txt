cmake_minimum_required(VERSION 3.23)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# note: CMAKE_BUILD_TYPE is set to Debug by default
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "[${CMAKE_BUILD_TYPE}] ")
message(STATUS "${CMAKE_CXX_COMPILER}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin-debug/")
else()
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin-release/")
endif()

project(
  tjson
  VERSION 0.1.0
  LANGUAGES CXX)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  add_executable("${PROJECT_NAME}" main.cc)
else()
  add_executable("${PROJECT_NAME}" test.cc)
endif()

target_include_directories(
  "${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/header-only/include/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
message("${CMAKE_CXX_FLAGS}")
