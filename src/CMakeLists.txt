cmake_minimum_required(VERSION 3.16.0)

message("ROOT CMakeList.txt")

# specify the C++ standard
set (CMAKE_CXX_STANDARD 14)

# set the project name
#get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
#string(REPLACE " " "_" ProjectId ${ProjectId})
#project(${CMAKE_PROJECT_NAME})

project(
  SNN-TSP
  VERSION 0.1
  DESCRIPTION "SNN-TSP with 6T2R neuromorphic device"
  LANGUAGES CXX)

# Get json library
include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(json)

# The compiled library code is here
add_subdirectory(utils)

# The executable code is here
add_subdirectory(tsp)
add_subdirectory(chip)