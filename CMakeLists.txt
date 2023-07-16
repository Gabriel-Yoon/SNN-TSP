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

# Build utils, tsp and snn_tsp targets
add_subdirectory(src/utils)
add_subdirectory(src/tsp)
add_subdirectory(src/chip)

# Set the output folder name with the current time
string(TIMESTAMP CURRENT_TIME "%Y-%m-%d-%H-%M-%S")
set(GLOB OUTPUT_FOLDER "output_${CURRENT_TIME}")

# Create the output directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER})

# Set the output directory for tsp and snn_tsp
set_target_properties(tsp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER})
set_target_properties(snn_tsp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER})

# Copy the tsp and snn_tsp executables to the output folder
#add_custom_command(
#    TARGET tsp POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tsp> ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/$<TARGET_FILE_NAME:tsp>
#    COMMENT "Copying tsp executable to the output folder"
#)

#add_custom_command(
#    TARGET snn_tsp POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:snn_tsp> ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/$<TARGET_FILE_NAME:snn_tsp>
#    COMMENT "Copying snn_tsp executable to the output folder"
#)

# Run the tsp and snn_tsp executables from the output folder
#add_custom_target(run_tsp
#    COMMAND ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/tsp/$<TARGET_FILE_NAME:tsp>
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
#    COMMENT "Running tsp"
#)

#add_custom_target(run_snn_tsp
#    COMMAND ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/snn_tsp/$<TARGET_FILE_NAME:snn_tsp>
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
#    COMMENT "Running snn_tsp"
#)
#
# Set snn_tsp_build target as a dependency of tsp_build
# add_dependencies(run_tsp run_snn_tsp)

# Create a custom target to run both tsp and snn_tsp
#add_custom_target(run
#    DEPENDS run_tsp run_snn_tsp
#    COMMENT "Running tsp and snn_tsp"
#)