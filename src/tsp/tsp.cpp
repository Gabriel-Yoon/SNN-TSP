#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "tsp.h"

using namespace std;
using json = nlohmann::json;

extern double INIT_PERFORMANCE = 0;
namespace csp {
    tsp::tsp(const char* param_file){
        // param_file == "tsp_data_26.json"
        std::ifstream f(param_file);
        json _TSPparam = json::parse(f);
        std::cout << "TSP PARSING --------" << std::endl;
        _numCity = _TSPparam["num_city"];
        std::cout << _numCity << std::endl;
        
        _solutionDistance = _TSPparam["solution_distance"]; // 19 for 5 cities, 937 for 26 cities.
        _solveMode = _TSPparam["mode"];
        
        // TSP Inhibition Weight Setup
        _ExciteSameWTADiffCities = _TSPparam["sameWTADiffCities"];
        _InhibitAdjWTASameCities = _TSPparam["adjWTASameCities"];
        _InhibitNonAdjWTASameCities = _TSPparam["nonAdjWTASameCities"];
        _b = _TSPparam["b"];
        _grad = _TSPparam["grad"];
        
        std::cout << "TSP PARSING COMPLETE" << std::endl;

        SetNumNeurons();
        SetWeightMatrix();
        
        const char* filename = "";
        std::string filename_str;
        filename_str += param_file;
        filename_str += "_weight";
        exportWeightToFile(filename_str);
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
        ofstream exportFile_weight_setup;
        exportFile_weight_setup.open("weight_setup.csv");
        // checking each element of matrix 
        // if it is greater than maxElement, 
        // update maxElement

        // Answer : 1 - 3 - 2 - 5 - 4
        distance_matrix = {
            {0, 3, 4, 2, 7},
            {3, 0, 4, 6, 3},
            {4, 4, 0, 5, 8},
            {2, 6, 5, 0, 6},
            {7, 3, 8, 6, 0},
        };

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

                if (v_WTA == h_WTA) { // same WTA
                    if (v_city == h_city) { // same city
                        weight_matrix[v_idx][h_idx].Gp = 0;
                        weight_flag = "0";
                    }
                    else { // in the same_WTA but btw different cities
                        weight_matrix[v_idx][h_idx].Gp = _ExciteSameWTADiffCities;
                        weight_flag = "-";
                    }
                }
                else if (fabs((v_WTA)-(h_WTA)) == 1 || fabs((v_WTA)-(h_WTA)) == (_numCity - 1)) { // adjacent WTA
                    if (v_city == h_city) { // adjacent WTA - same city = inhibition!
                        weight_matrix[v_idx][h_idx].Gp = _InhibitAdjWTASameCities;
                        weight_flag = "/";
                    }
                    else { // THE MOST IMPORTANT PART OF THE TRAVELING SALESMAN PROBLEM
                        weight_matrix[v_idx][h_idx].Gp = _b + (1 - distance / max_distance) * _grad;
                        weight_flag = "*";
                    }
                }
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
                //cout << weight_flag;
                exportFile_weight_setup << weight_matrix[v_idx][h_idx].Gp << ",";
            }
            exportFile_weight_setup << "\n";
            //printf("\n");
        }
        //printf("\n");
        exportFile_weight_setup.close();
        cout << "[_END_] WEIGHT SETUP" << endl;
    }

    /*
    inline void tsp::SetWeightMatrixBilateralBM() {
        return 0;
    }
    */

    void tsp::exportWeightToFile(std::string& filename){
        auto result = nlohmann::json{
        {"weight", json::array()},
        };

        for (auto i = 0; i < num_neurons[side_v]; i++) {
            auto& outer = result["weight"];
            for (auto j = 0; j < num_neurons[side_h]; j++) {
                // You'll need to convert whatever Items is into a JSON type first
                outer[i].push_back(weight_matrix[i][j].Gp);
            }
        }

        std::ofstream out(filename + ".json");
        out << result;
    }

    /*
    void tsp::wta_initialize(double* potential[], double init_val) {
        // First WTA zeroing process
        
        for (int i = 0; i < _numCity; i++) {
            potential[side_h][i] = init_val; // init_val should be param.pt_init
        }
        
    }
    */



    /*
            // FIVE is a set of 5 cities. The minimal tour has length 19.
            // Answer : 1 - 3 - 2 - 5 - 4
            distance_matrix = {
                {0, 3, 4, 2, 7},
                {3, 0, 4, 6, 3},
                {4, 4, 0, 5, 8},
                {2, 6, 5, 0, 6},
                {7, 3, 8, 6, 0},
            };


            // FRI26 TSP dataset from "https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html"
            // Answer : 1-25-24-23-26-22-21-17-18-20-19-16-11-12-13-15-14-10-9-8-7-5-6-4-3-2-1 (937)
            distance_matrix = {
                {0,   83,  93, 129, 133, 139, 151, 169, 135, 114, 110,  98,  99,  95,  81, 152, 159, 181, 172, 185, 147, 157, 185, 220, 127, 181},
                {83,   0,  40,  53,  62,  64,  91, 116,  93,  84,  95,  98,  89,  68,  67, 127, 156, 175, 152, 165, 160, 180, 223, 268, 179, 197},
                {93,  40,   0,  42,  42,  49,  59,  81,  54,  44,  58,  64,  54,  31,  36,  86, 117, 135, 112, 125, 124, 147, 193, 241, 157, 161},
                {129,  53,  42,   0,  11,  11,  46,  72,  65,  70,  88, 100,  89,  66,  76, 102, 142, 156, 127, 139, 155, 180, 228, 278, 197, 190},
                {133,  62,  42,  11,   0,   9,  35,  61,  55,  62,  82,  95,  84,  62,  74,  93, 133, 146, 117, 128, 148, 173, 222, 272, 194, 182},
                {139,  64,  49,  11,   9,   0,  39,  65,  63,  71,  90, 103,  92,  71,  82, 100, 141, 153, 124, 135, 156, 181, 230, 280, 202, 190},
                {151,  91,  59,  46,  35,  39,   0,  26,  34,  52,  71,  88,  77,  63,  78,  66, 110, 119,  88,  98, 130, 156, 206, 257, 188, 160},
                {169, 116,  81,  72,  61,  65,  26,   0,  37,  59,  75,  92,  83,  76,  91,  54,  98, 103,  70,  78, 122, 148, 198, 250, 188, 148},
                {135,  93,  54,  65,  55,  63,  34,  37,   0,  22,  39,  56,  47,  40,  55,  37,  78,  91,  62,  74,  96, 122, 172, 223, 155, 128},
                {114,  84,  44,  70,  62,  71,  52,  59,  22,   0,  20,  36,  26,  20,  34,  43,  74,  91,  68,  82,  86, 111, 160, 210, 136, 121},
                {110,  95,  58,  88,  82,  90,  71,  75,  39,  20,   0,  18,  11,  27,  32,  42,  61,  80,  64,  77,  68,  92, 140, 190, 116, 103},
                {98,  98,  64, 100,  95, 103,  88,  92,  56,  36,  18,   0,  11,  34,  31,  56,  63,  85,  75,  87,  62,  83, 129, 178, 100,  99},
                {99,  89,  54,  89,  84,  92,  77,  83,  47,  26,  11,  11,   0,  23,  24,  53,  68,  89,  74,  87,  71,  93, 140, 189, 111, 107},
                {95,  68,  31,  66,  62,  71,  63,  76,  40,  20,  27,  34,  23,   0,  15,  62,  87, 106,  87, 100,  93, 116, 163, 212, 132, 130},
                {81,  67,  36,  76,  74,  82,  78,  91,  55,  34,  32,  31,  24,  15,   0,  73,  92, 112,  96, 109,  93, 113, 158, 205, 122, 130},
                {152, 127,  86, 102,  93, 100,  66,  54,  37,  43,  42,  56,  53,  62,  73,   0,  44,  54,  26,  39,  68,  94, 144, 196, 139,  95},
                {159, 156, 117, 142, 133, 141, 110,  98,  78,  74,  61,  63,  68,  87,  92,  44,   0,  22,  34,  38,  30,  53, 102, 154, 109,  51},
                {181, 175, 135, 156, 146, 153, 119, 103,  91,  91,  80,  85,  89, 106, 112,  54,  22,   0,  33,  29,  46,  64, 107, 157, 125,  51},
                {172, 152, 112, 127, 117, 124,  88,  70,  62,  68,  64,  75,  74,  87,  96,  26,  34,  33,   0,  13,  63,  87, 135, 186, 141,  81},
                {185, 165, 125, 139, 128, 135,  98,  78,  74,  82,  77,  87,  87, 100, 109,  39,  38,  29,  13,   0,  68,  90, 136, 186, 148,  79},
                {147, 160, 124, 155, 148, 156, 130, 122,  96,  86,  68,  62,  71,  93,  93,  68,  30,  46,  63,  68,   0,  26,  77, 128,  80,  37},
                {157, 180, 147, 180, 173, 181, 156, 148, 122, 111,  92,  83,  93, 116, 113,  94,  53,  64,  87,  90,  26,   0,  50, 102,  65,  27},
                {185, 223, 193, 228, 222, 230, 206, 198, 172, 160, 140, 129, 140, 163, 158, 144, 102, 107, 135, 136,  77,  50,   0,  51,  64,  58},
                {220, 268, 241, 278, 272, 280, 257, 250, 223, 210, 190, 178, 189, 212, 205, 196, 154, 157, 186, 186, 128, 102,  51,   0,  93, 107},
                {127, 179, 157, 197, 194, 202, 188, 188, 155, 136, 116, 100, 111, 132, 122, 139, 109, 125, 141, 148,  80,  65,  64,  93,   0,  90},
                {181, 197, 161, 190, 182, 190, 160, 148, 128, 121, 103,  99, 107, 130, 130,  95,  51,  51,  81,  79,  37,  27,  58, 107,  90,   0},
            };

            /*
            // GR17 TSP dataset from "https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html"
            // Answer : 1-4-13-7-8-56-17-14-15-3-11-10-2-5-9-12-16 (2085)
            distance_matrix = {
                {  0, 633, 257,  91, 412, 150,  80, 134, 259, 505, 353, 324,  70, 211, 268, 246, 121},
                {633,   0, 390, 661, 227, 488, 572, 530, 555, 289, 282, 638, 567, 466, 420, 745, 518},
                {257, 390,   0, 228, 169, 112, 196, 154, 372, 262, 110, 437, 191,  74,  53, 472, 142},
                { 91, 661, 228,   0, 383, 120,  77, 105, 175, 476, 324, 240,  27, 182, 239, 237,  84},
                {412, 227, 169, 383,   0, 267, 351, 309, 338, 196,  61, 421, 346, 243, 199, 528, 297},
                {150, 488, 112, 120, 267,   0,  63,  34, 264, 360, 208, 329,  83, 105, 123, 364,  35},
                { 80, 572, 196,  77, 351,  63,   0,  29, 232, 444, 292, 297,  47, 150, 207, 332,  29},
                {134, 530, 154, 105, 309,  34,  29,   0, 249, 402, 250, 314,  68, 108, 165, 349,  36},
                {259, 555, 372, 175, 338, 264, 232, 249,   0, 495, 352,  95, 189, 326, 383, 202, 236},
                {505, 289, 262, 476, 196, 360, 444, 402, 495,   0, 154, 578, 439, 336, 240, 685, 390},
                {353, 282, 110, 324,  61, 208, 292, 250, 352, 154,   0, 435, 287, 184, 140, 542, 238},
                {324, 638, 437, 240, 421, 329, 297, 314,  95, 578, 435,   0, 254, 391, 448, 157, 301},
                { 70, 567, 191,  27, 346,  83,  47,  68, 189, 439, 287, 254,   0, 145, 202, 289,  55},
                {211, 466,  74, 182, 243, 105, 150, 108, 326, 336, 184, 391, 145,   0,  57, 426,  96},
                {268, 420,  53, 239, 199, 123, 207, 165, 383, 240, 140, 448, 202,  57,   0, 483, 153},
                {246, 745, 472, 237, 528, 364, 332, 349, 202, 685, 542, 157, 289, 426, 483,   0, 336},
                {121, 518, 142, 84 ,297 , 35 , 29 , 36 , 236, 390, 238, 301,  55,  96, 153, 336,   0},
            };



            // TSP dataset from "https://developers.google.com/optimization/routing/tsp#c++"
            distance_matrix = {
                {0, 2451, 713, 1018, 1631, 1374, 2408, 213, 2571, 875, 1420, 2145, 1972},
                {2451, 0, 1745, 1524, 831, 1240, 959, 2596, 403, 1589, 1374, 357, 579},
                {713, 1745, 0, 355, 920, 803, 1737, 851, 1858, 262, 940, 1453, 1260},
                {1018, 1524, 355, 0, 700, 862, 1395, 1123, 1584, 466, 1056, 1280, 987},
                {1631, 831, 920, 700, 0, 663, 1021, 1769, 949, 796, 879, 586, 371},
                {1374, 1240, 803, 862, 663, 0, 1681, 1551, 1765, 547, 225, 887, 999},
                {2408, 959, 1737, 1395, 1021, 1681, 0, 2493, 678, 1724, 1891, 1114, 701},
                {213, 2596, 851, 1123, 1769, 1551, 2493, 0, 2699, 1038, 1605, 2300, 2099},
                {2571, 403, 1858, 1584, 949, 1765, 678, 2699, 0, 1744, 1645, 653, 600},
                {875, 1589, 262, 466, 796, 547, 1724, 1038, 1744, 0, 679, 1272, 1162},
                {1420, 1374, 940, 1056, 879, 225, 1891, 1605, 1645, 679, 0, 1017, 1200},
                {2145, 357, 1453, 1280, 586, 887, 1114, 2300, 653, 1272, 1017, 0, 504},
                {1972, 579, 1260, 987, 371, 999, 701, 2099, 600, 1162, 1200, 504, 0},
            };
            */


}
