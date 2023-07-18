#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "tsp.h"

using namespace std;
using json = nlohmann::json;

extern double INIT_PERFORMANCE = 0;
namespace csp {

    tsp::tsp(const std::string& tsp_json_file_path, const std::string& tsp_matrix_file_path, const std::string& tsp_itinerary_file_path){
        
        read_distance_matrix(tsp_matrix_file_path);
        load_itinerary(tsp_itinerary_file_path);
        calculate_minimal_total_distance();
        read_and_write_json_file(tsp_json_file_path);

        
        // param_file == "tsp_data.json"
        std::ifstream f(tsp_json_file_path);
        json _TSPparam = json::parse(f);
        std::cout << "TSP PARSING --------" << std::endl;
        std::cout << "number of cities are : " << _numCity << std::endl;
        _solveMode = _TSPparam["mode"];
        
        // TSP Inhibition Weight Setup
        _ExciteSameWTADiffCities = _TSPparam["sameWTADiffCities"];
        _InhibitAdjWTASameCities = _TSPparam["adjWTASameCities"];
        _InhibitNonAdjWTASameCities = _TSPparam["nonAdjWTASameCities"];
        _b = _TSPparam["b"];
        _grad = _TSPparam["grad"];
        
        std::cout << "TSP PARSING COMPLETE" << std::endl;
    }

    void tsp::initialize() {
        SetNumNeurons();
        SetWeightMatrix();

        std::string _weightFilename = "weight";
        exportWeightToFile(_weightFilename);
    }

    // Set number of neurons in v,h layer
    inline void tsp::SetNumNeurons() {
        if (_solveMode == "bilateral_BM") {
            if (_numCity % 2 == 0) { // even number of city
                num_neurons[side_v] = (_numCity / 2) * (_numCity / 2);
                num_neurons[side_h] = (_numCity / 2) * (_numCity / 2) / 4;
            }
            else { // odd number of city
                num_neurons[side_v] = ((_numCity + 1) / 2) * ((_numCity + 1) / 2);
                num_neurons[side_h] = ((_numCity + 1) / 2) * ((_numCity + 1) / 2);
            }
        } else if (_solveMode == "lateral_BM") {
            num_neurons[side_v] = _numCity * _numCity;
            num_neurons[side_h] = _numCity * _numCity;
        }
    }

    inline void tsp::SetWeightMatrix() {
        
        weight_matrix.resize(num_neurons[side_v]);
        for (int i = 0; i < num_neurons[side_v]; i++)
        {
            weight_matrix[i].resize(num_neurons[side_h]);
        }

        if (_solveMode == "lateral_BM") {
            SetWeightMatrixLateralBM();
        }
        else if (_solveMode == "bilateral_BM") {
            //SetWeightMatrixBilateralBM();
        }

    }

    inline void tsp::SetWeightMatrixLateralBM() {

        // Initializing max element as INT_MIN 
        double max_distance = 0;

        for (int i = 0; i < _numCity; i++) {
            for (int j = 0; j < _numCity; j++) {
                if (distance_matrix[i][j] > max_distance) {
                    max_distance = distance_matrix[i][j];
                }
            }
        }

        /*

               (lateral) EXAMPLE OF WEIGHT MATRIX FOR TSP OF 7 CITIES

                                                      H I D D E N
                        STEP 1                STEP 2                STEP 3                STEP 4                STEP 5                STEP 6                STEP 7
                 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7
               |---------------------------------------------------------------------------------------------------------------------------------------------------------
              1| h  -  -  -  -  -  - | h  -  -  -  -  -  - | h  -  -  -  -  -  - | h  -  -  -  -  -  - | h  -  -  -  -  -  - | h  -  -  -  -  -  - | H  E  E  E  E  E  E
           S  2| -  h  -  -  -  -  - | -  h  -  -  -  -  - | -  h  -  -  -  -  - | -  h  -  -  -  -  - | -  h  -  -  -  -  - | -  h  -  -  -  -  - | E  H  E  E  E  E  E
           T  3| -  -  h  -  -  -  - | -  -  h  -  -  -  - | -  -  h  -  -  -  - | -  -  h  -  -  -  - | -  -  h  -  -  -  - | -  -  h  -  -  -  - | E  E  H  E  E  E  E
           E  4| E  E  E  H  E  E  E | -  -  -  h  -  -  - | -  -  -  h  -  -  - | -  -  -  h  -  -  - | -  -  -  h  -  -  - | -  -  -  h  -  -  - | E  E  E  H  E  E  E
           P  5| E  E  E  E  H  E  E | -  -  -  -  h  -  - | -  -  -  -  h  -  - | -  -  -  -  h  -  - | -  -  -  -  h  -  - | -  -  -  -  h  -  - | E  E  E  E  H  E  E
           1  6| E  E  E  E  E  H  E | -  -  -  -  -  h  - | -  -  -  -  -  h  - | -  -  -  -  -  h  - | -  -  -  -  -  h  - | -  -  -  -  -  h  - | E  E  E  E  E  H  E
              7| E  E  E  E  E  E  H | -  -  -  -  -  -  h | -  -  -  -  -  -  h | -  -  -  -  -  -  h | -  -  -  -  -  -  h | -  -  -  -  -  -  h | E  E  E  E  E  E  H
              =| ====================|=====================|=====================|=====================
              1| H  E  E  E  E  E  E | H  E  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
           S  2| E  H  E  E  E  E  E | E  H  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -                to be continued
           T  3| E  E  H  E  E  E  E | E  E  H  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
           E  4| E  E  E  H  E  E  E | E  E  E  H  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        V  P  5| E  E  E  E  H  E  E | E  E  E  E  H  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        I  2  6| E  E  E  E  E  H  E | E  E  E  E  E  H  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        S     7| E  E  E  E  E  E  H | E  E  E  E  E  E  H | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        I     =| ====================|=====================|=====================|=====================
        B     1| -  -  -  -  -  -  - | H  E  E  E  E  E  E | H  E  E  E  E  E  E | -  -  -  -  -  -  -
        L  S  2| -  -  -  -  -  -  - | E  H  E  E  E  E  E | E  H  E  E  E  E  E | -  -  -  -  -  -  -
        E  T  3| -  -  -  -  -  -  - | E  E  H  E  E  E  E | E  E  H  E  E  E  E | -  -  -  -  -  -  -
           E  4| -  -  -  -  -  -  - | E  E  E  H  E  E  E | E  E  E  H  E  E  E | -  -  -  -  -  -  -
           P  5| -  -  -  -  -  -  - | E  E  E  E  H  E  E | E  E  E  E  H  E  E | -  -  -  -  -  -  -
           3  6| -  -  -  -  -  -  - | E  E  E  E  E  H  E | E  E  E  E  E  H  E | -  -  -  -  -  -  -
              7| -  -  -  -  -  -  - | E  E  E  E  E  E  H | E  E  E  E  E  E  H | -  -  -  -  -  -  -
              =| ====================|=====================|=====================|=====================
              1| -  -  -  -  -  -  - | -  -  -  -  -  -  - | H  E  E  E  E  E  E | 1  0  0  0  0  0  0
           S  2| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  H  E  E  E  E  E | 0  1  0  0  0  0  0
           T  3| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  H  E  E  E  E | 0  0  1  0  0  0  0
           E  4| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  H  E  E  E | 0  0  0  1  0  0  0
           P  5| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  1  0  0
           4  6| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  0  1  0
              7| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  0  0  1

            D : EXCITATION  | _ExciteSameWTADiffCities
            H : INHIBITION  | _InhibitAdjWTASameCities
            h : INHIBITION  | _InhibitNonAdjWTASameCities -> not applicable!
            - : UNDEFINED   | This would be 0 for now




                (bilateral) EXAMPLE OF WEIGHT MATRIX FOR TSP OF 7 CITIES

                                                      H I D D E N     
                        STEP 2                STEP 4                STEP 6                STEP 7'
                 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 | 1  2  3  4  5  6  7 
               |---------------------------------------------------------------------------------------
              1| H  E  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | H  E  E  E  E  E  E
           S  2| E  H  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  H  E  E  E  E  E
           T  3| E  E  H  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  H  E  E  E  E
           E  4| E  E  E  H  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  H  E  E  E
           P  5| E  E  E  E  H  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E
           1  6| E  E  E  E  E  H  E | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  E  H  E
              7| E  E  E  E  E  E  H | -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  E  E  H
              =| ====================|=====================|=====================|=====================
              1| H  E  E  E  E  E  E | H  E  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
           S  2| E  H  E  E  E  E  E | E  H  E  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
           T  3| E  E  H  E  E  E  E | E  E  H  E  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
           E  4| E  E  E  H  E  E  E | E  E  E  H  E  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        V  P  5| E  E  E  E  H  E  E | E  E  E  E  H  E  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        I  3  6| E  E  E  E  E  H  E | E  E  E  E  E  H  E | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        S     7| E  E  E  E  E  E  H | E  E  E  E  E  E  H | -  -  -  -  -  -  - | -  -  -  -  -  -  -
        I     =| ====================|=====================|=====================|=====================
        B     1| -  -  -  -  -  -  - | H  E  E  E  E  E  E | H  E  E  E  E  E  E | -  -  -  -  -  -  -
        L  S  2| -  -  -  -  -  -  - | E  H  E  E  E  E  E | E  H  E  E  E  E  E | -  -  -  -  -  -  -
        E  T  3| -  -  -  -  -  -  - | E  E  H  E  E  E  E | E  E  H  E  E  E  E | -  -  -  -  -  -  -
           E  4| -  -  -  -  -  -  - | E  E  E  H  E  E  E | E  E  E  H  E  E  E | -  -  -  -  -  -  -
           P  5| -  -  -  -  -  -  - | E  E  E  E  H  E  E | E  E  E  E  H  E  E | -  -  -  -  -  -  -
           5  6| -  -  -  -  -  -  - | E  E  E  E  E  H  E | E  E  E  E  E  H  E | -  -  -  -  -  -  -
              7| -  -  -  -  -  -  - | E  E  E  E  E  E  H | E  E  E  E  E  E  H | -  -  -  -  -  -  -
              =| ====================|=====================|=====================|=====================
              1| -  -  -  -  -  -  - | -  -  -  -  -  -  - | H  E  E  E  E  E  E | 1  0  0  0  0  0  0
           S  2| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  H  E  E  E  E  E | 0  1  0  0  0  0  0
           T  3| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  H  E  E  E  E | 0  0  1  0  0  0  0
           E  4| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  H  E  E  E | 0  0  0  1  0  0  0
           P  5| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  1  0  0
           7  6| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  0  1  0
              7| -  -  -  -  -  -  - | -  -  -  -  -  -  - | E  E  E  E  H  E  E | 0  0  0  0  0  0  1
         
            D : EXCITATION  | _ExciteSameWTADiffCities
            H : INHIBITION  | _InhibitAdjWTASameCities
            h : INHIBITION  | _InhibitNonAdjWTASameCities -> not applicable!
            - : UNDEFINED   | This would be 0 for now
         */

        cout << "[START] WEIGHT SETUP" << endl;
        int v_WTA, v_city, h_WTA, h_city;
        // i : WTA Network, j : City
        for (int v_idx = 0; v_idx < num_neurons[side_v]; v_idx++) {
            for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++) {

                double max_w = 10;
                double min_w = 0;
                v_WTA = v_idx / _numCity + 1;
                v_city = v_idx % _numCity + 1;
                h_WTA = h_idx / _numCity + 1;
                h_city = h_idx % _numCity + 1;

                double distance = distance_matrix[v_city - 1][h_city - 1];
                std::string weight_flag;

                if (v_WTA == h_WTA) { // same WTA===============================================================
                    if (v_city == h_city) { // same city
                        weight_matrix[v_idx][h_idx].Gp = 0;
                        weight_flag = "0";
                    }
                    else { // in the same_WTA but btw different cities
                        weight_matrix[v_idx][h_idx].Gp = _ExciteSameWTADiffCities;
                        weight_flag = "-";
                    }
                } // different WTA but neighbor===============================================================
                else if (fabs((v_WTA)-(h_WTA)) == 1 || fabs((v_WTA)-(h_WTA)) == (_numCity - 1)) { // adjacent WTA
                    if (v_city == h_city) { // adjacent WTA - same city = inhibition!
                        weight_matrix[v_idx][h_idx].Gp = _InhibitAdjWTASameCities;
                        weight_flag = "/";
                    }
                    else { // THE MOST IMPORTANT PART OF THE TRAVELING SALESMAN PROBLEM
                        weight_matrix[v_idx][h_idx].Gp = _b + (1 - distance / max_distance) * _grad;
                        weight_flag = "*";
                    }
                } // different WTA not neighbor===============================================================
                else if (fabs((v_WTA % _numCity) - (h_WTA % _numCity)) != 1) { //non-adjacent WTA networks
                    if (v_city == h_city) {
                        weight_matrix[v_idx][h_idx].Gp = _InhibitNonAdjWTASameCities; // inhibition btw same cities
                        weight_flag = "|";
                    }
                    else { // no connection
                        weight_matrix[v_idx][h_idx].Gp = 0;
                        weight_flag = "0";
                    }
                }

                if (v_idx == 0 && h_city == 1) {
                    weight_matrix[v_idx][h_idx].Gp = -0.3;
                }

                // Check for weight value if they touch the boundaries
                double weight = weight_matrix[v_idx][h_idx].Gp;
                if (weight > max_w) {
                    weight = max_w;
                }
                else if (weight < min_w) {
                    weight = min_w;
                }
            }
        }
        cout << "[_END_] WEIGHT SETUP" << endl;
    }

    void tsp::exportWeightToFile(std::string filename){
        auto result = nlohmann::json{
        {"weight_gp", json::array()},
        {"weight_gm", json::array()},
        };

        for (auto i = 0; i < num_neurons[side_v]; i++) {
            auto& _Gp = result["weight_gp"];
            auto& _Gm = result["weight_gm"];
            for (auto j = 0; j < num_neurons[side_h]; j++) {
                // You'll need to convert whatever Items is into a JSON type first
                _Gp[i].push_back(weight_matrix[i][j].Gp);
                _Gm[i].push_back(weight_matrix[i][j].Gm);
            }
        }
        std::string _outputfilename = filename + ".json";
        std::ofstream out(_outputfilename);
        out << result;
    }

    // Function to update the distance_matrix in the JSON data
    // Function to read and write JSON data to a file
    void tsp::read_and_write_json_file(const std::string& file_path) {
        // Read the existing JSON data
        json data;
        std::ifstream read_file(file_path);
        if (read_file) {
            read_file >> data;
        }

        // Update or create the distance_matrix in the JSON data
        data["distance_matrix"] = distance_matrix;
        data["num_city"] = _numCity;
        data["optimal_itinerary"] = _itinerary;
        data["solution"] = this->_solutionDistance;

        // Write the updated JSON data back to the file
        std::ofstream write_file(file_path);
        if (write_file) {
            write_file << std::setw(4) << data << std::endl;
        }
    }

    // Function to read the distance matrix from the input file and count the number of cities (rows)
    void tsp::read_distance_matrix(const std::string& file_path) {
        std::ifstream file(file_path);
        _numCity = 0; // Initialize the number of cities (rows) count
        if (file) {
            std::string line;
            while (std::getline(file, line)) {
                std::vector<double> row;
                std::istringstream iss(line);
                double value;
                while (iss >> value) {
                    row.push_back(value);
                }
                distance_matrix.push_back(row);
                _numCity++; // Increment the number of cities count for each row read
            }
        }
    }

    // Function to load the itinerary data from the txt file and find the optimal itinerary
    void tsp::load_itinerary(const std::string& file_path) {
        std::ifstream file(file_path);
        if (file) {
            int city;
            while (file >> city) {
                _itinerary.push_back(city);
            }
        }
    }

    // Function to calculate the minimal total distance following the route of the itinerary data
    void tsp::calculate_minimal_total_distance() {
        this->_solutionDistance = 0.0;
        for (size_t i = 1; i < this->_itinerary.size(); ++i) {
            int from_city = this->_itinerary[i - 1] - 1;
            int to_city = this->_itinerary[i] - 1;
            this->_solutionDistance += this->distance_matrix[from_city][to_city];
        }

        // Add the distance to return to the starting city
        int last_city = this->_itinerary.back() - 1;
        int start_city = this->_itinerary.front() - 1;
        this->_solutionDistance += this->distance_matrix[last_city][start_city];
    }

    void tsp::copy_json_file(const std::string& source_file_path, const std::string& destination_file_path) {
        // Read the existing JSON data
        std::ifstream source_file(source_file_path);
        json data;
        if (source_file) {
            source_file >> data;
        }
        else {
            std::cerr << "Error: Unable to read the source JSON file." << std::endl;
            return;
        }

        // Write the JSON data to the destination file
        std::ofstream destination_file(destination_file_path);
        if (destination_file) {
            destination_file << std::setw(4) << data << std::endl;
            std::cout << "JSON file copied successfully." << std::endl;
        }
        else {
            std::cerr << "Error: Unable to write the destination JSON file." << std::endl;
        }
    }

}
