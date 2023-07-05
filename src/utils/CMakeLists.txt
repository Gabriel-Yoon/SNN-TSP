message("src/utils/CMakeList.txt")

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

# make snn_lib
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${SNN-TSP_SOURCE_DIR}/utils/*.cpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${SNN-TSP_SOURCE_DIR}/utils/*.hpp")
add_library(snn_lib ${SOURCE_LIST} ${HEADER_LIST})
target_link_libraries(snn_lib PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(snn_lib PUBLIC ${snn_lib_INCLUDE_DIRS})
