cmake_minimum_required(VERSION 3.23)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# note: CMAKE_BUILD_TYPE is set to empty by default
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin-debug/")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin-release/")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

project(
  tjson
  VERSION 0.1.8
  LANGUAGES CXX)

add_executable("${PROJECT_NAME}-test" test.cc)
add_executable("${PROJECT_NAME}" CLI.cc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/header-only/include/")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
message(STATUS "[${CMAKE_BUILD_TYPE}] ")
message(STATUS "${CMAKE_CXX_COMPILER}")
message("${CMAKE_CXX_FLAGS}")
