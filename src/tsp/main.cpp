#include <iostream>
#include <filesystem>

#include "tsp.h"

int main(int argc, char* argv[]) {
    std::filesystem::path CurrentPath = std::filesystem::current_path();
    CurrentPath /= "./tsp_data.json";
    // const char* _TSPDataJsonFilePath = CurrentPath.string().c_str();
    //std::cout << _TSPDataJsonFilePath << std::endl;
    std::string _TSPDataJsonFilePath = "/Users/gabriel/Development/SNN-TSP/src/tsp/tsp_data.json";
    std::cout << _TSPDataJsonFilePath << std::endl;

    // For personal Windows computer, the absolute path for the tsp_data.json is below
    //_TSPDataJsonFilePath = "C:/Users/Gabriel/dev/test/tsp/tsp_data.json";

    // For personal Mac computer, the absolute path for the tsp_data.json is below

    csp::tsp tspsolver(_TSPDataJsonFilePath);
    std::cout << "tsp compile done" << std::endl;

    // Time print 1
    
    return 0;
}