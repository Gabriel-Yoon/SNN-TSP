#include <iostream>
#include <filesystem>

#include "tsp.h"

int main(int argc, char* argv[]) {
    std::filesystem::path CurrentPath = std::filesystem::current_path();
    CurrentPath /= "./tsp_data.json";
    // const char* _TSPDataJsonFilePath = CurrentPath.string().c_str();
    //std::cout << _TSPDataJsonFilePath << std::endl;

    const std::string json_file_path = "tsp_data.json";

    const std::string tsp_json_file_path = "tsp_data.json";
    const std::string tsp_matrix_file_path = "tsp_matrix.txt";
    const std::string tsp_itinerary_file_path = "tsp_itinerary.txt";
    // For personal Windows computer, the absolute path for the tsp_data.json is below
    //_TSPDataJsonFilePath = "C:/Users/Gabriel/dev/test/tsp/tsp_data.json";

    csp::tsp tspsolver(tsp_json_file_path, tsp_matrix_file_path, tsp_itinerary_file_path);
    tspsolver.initialize();
    tspsolver.read_and_write_json_file(tsp_json_file_path);
    tspsolver.copy_json_file(tsp_json_file_path, "tsp_data.json");
    std::cout << "TSP weight setting complete and file saved" << std::endl;

    // Time print 1
    
    return 0;
}