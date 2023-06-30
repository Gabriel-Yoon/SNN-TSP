cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 11)
project(SNN-TSP VERSION 0.1.0)

find_package(Boost)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include(CTest)
enable_testing()

add_executable(tspsim tester.cpp core.cpp export.cpp neuron_control.cpp simulation_parameters.cpp spike_control.cpp synapse_setting.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
