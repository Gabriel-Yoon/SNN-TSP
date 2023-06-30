cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 14)

#find_package(Boost)

#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#endif()

#include(CTest)
#enable_testing()

include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.10.5
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(json)

target_link_libraries(${PROJECT_NAME}
    nlohmann_json::nlohmann_json)

FetchContent_Declare(NumCpp
        GIT_REPOSITORY https://github.com/dpilger26/NumCpp
        GIT_TAG Version_2.11.0)
FetchContent_MakeAvailable(NumCpp)

target_link_libraries(${PROJECT_NAME}
    NumCpp::NumCpp
)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

file(GLOB SOURCES  *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} )