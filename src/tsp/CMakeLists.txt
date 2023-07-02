message("src/tsp/CMakeList.txt")

# make snn_lib
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${SNN-TSP_SOURCE_DIR}/tsp/*.cpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${SNN-TSP_SOURCE_DIR}/tsp/*.hpp")
add_library(snn_lib ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(snn_lib PUBLIC ../utils)
target_link_libraries(snn_lib PUBLIC
    nlohmann_json::nlohmann_json)