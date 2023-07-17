message("src/chip/CMakeList.txt")

project(snn_tsp)
# specify the C++ standard
set (CMAKE_CXX_STANDARD 14)

# Get json library
include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(json)

# make executable
file(GLOB SNN_TSP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(snn_tsp ${SNN_TSP_SOURCES})

# include files in ../utils
target_include_directories(snn_tsp PUBLIC "${SNN-TSP_SOURCE_DIR}/utils")
target_link_libraries(snn_tsp PRIVATE snn_lib nlohmann_json::nlohmann_json)