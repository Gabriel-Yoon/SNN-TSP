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
message(${CURRENT_TIME})
set(OUTPUT_FOLDER "output_${CURRENT_TIME}")

# Create the output directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER})

# Custom target to run both tsp and snn_tsp
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tsp> ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/$<TARGET_FILE_NAME:tsp>
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:snn_tsp> ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/$<TARGET_FILE_NAME:snn_tsp>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/tsp/tsp_data.json ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/tsp/tsp_matrix.txt ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/tsp/tsp_itinerary.txt ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/chip/sim_params.test.json ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/plot ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/plot
    COMMAND $<TARGET_FILE:tsp> ${ARGV}
    COMMAND $<TARGET_FILE:snn_tsp> ${ARGV}
    COMMAND ${CMAKE_COMMAND} -E echo "Running main.py..."
    COMMAND ${CMAKE_COMMAND} -E env python3 ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}/plot/main.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    COMMENT "Running tsp and snn_tsp at ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}"
)

# Set snn_tsp_build target as a dependency of tsp_build
add_dependencies(tsp snn_tsp)

# Set run_all target as a dependency of the build_all target
add_custom_target(build_all
    DEPENDS run_all
    COMMENT "Building all targets"
)