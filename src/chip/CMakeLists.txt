message("src/chip/CMakeList.txt")

# make executable
file(GLOB SOURCES  *.cpp)
add_executable(snn_tsp ${SOURCES})

# include files in ../utils
target_include_directories(snn_tsp PUBLIC ../utils)

target_link_libraries(snn_tsp PRIVATE snn_lib
    nlohmann_json::nlohmann_json)