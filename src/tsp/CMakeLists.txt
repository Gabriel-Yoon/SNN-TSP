message("src/tsp/CMakeList.txt")

# specify the C++ standard
set (CMAKE_CXX_STANDARD 17)

# Get json library
include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(json)

# make executable
file(GLOB TSP_SOURCES "${PROJECT_SOURCE_DIR}/tsp/*.h"
"${PROJECT_SOURCE_DIR}/tsp/*.cpp")
add_executable(tsp ${TSP_SOURCES})
target_link_libraries(tsp nlohmann_json::nlohmann_json)