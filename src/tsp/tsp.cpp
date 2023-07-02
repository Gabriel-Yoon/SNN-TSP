
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "tsp.hpp"

using namespace std;
using json = nlohmann::json;

extern double INIT_PERFORMANCE = 0;
namespace csp {
    tsp::tsp(const char* param_file){
        // param_file == "tsp_data_26.json"
        std::ifstream f(param_file);
        json _TSPparam = json::parse(f);

        _numCity = _TSPparam["num_city"];
        _solutionDistance = _TSPparam["solution_distance"]; // 19 for 5 cities, 937 for 26 cities.
        _solveMode = _TSPparam["mode"];

        // TSP Inhibition Weight Setup
        _sameWTADiffCities = _TSPparam["sameWTADiffCities"];
        _adjWTASameCities = _TSPparam["adjWTASameCities"];
        _nonAdjWTASameCities = _TSPparam["nonAdjWTASameCities"];
        _b = _TSPparam["b"];
        _grad = _TSPparam["grad"];

        SetNumNeurons();
        SetWeightMatrix();
        
    }

    // Set number of neurons in v,h layer
    inline void tsp::SetNumNeurons() {
        if (_numCity % 2 == 0) { // even number of city
            num_neurons[side_v] = _numCity * _numCity;
            num_neurons[side_h] = _numCity * _numCity;
        }
        else { // odd number of city
            num_neurons[side_v] = _numCity * _numCity;
            num_neurons[side_h] = _numCity * _numCity;
        }
        /*
        core.num_neurons[side_v] = neurons_visible_city;
        core.num_neurons[side_h] = neurons_hidden_city;

        core.num_neurons_bias[0] = neurons_visible_bias;
        core.num_neurons_bias[1] = neurons_hidden_bias;
        */
    }

    inline void tsp::SetWeightMatrix() {
        if (_solveMode == "lateral_BM") {
            SetWeightMatrixLateralBM();
        }
        else if (_solveMode == "bilateral_BM") {
            SetWeightMatrixBilateralBM();
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

        for (int i = 0; i < num_city; i++) {
            for (int j = 0; j < num_city; j++) {
                if (distance_matrix[i][j] > max_distance) {
                    max_distance = distance_matrix[i][j];
                }
            }
        }

        cout << "[START] WEIGHT SETUP" << endl;
        int v_WTA, v_city, h_WTA, h_city;
        // i : WTA Network, j : City
        for (int v_idx = 0; v_idx < num_neurons[side_v]; v_idx++) {
            for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++) {

                double max_w = 10;
                double min_w = 0;
                v_WTA = v_idx / num_city + 1;
                v_city = v_idx % num_city + 1;
                h_WTA = h_idx / num_city + 1;
                h_city = h_idx % num_city + 1;

                double distance = distance_matrix[v_city - 1][h_city - 1];
                std::string weight_flag;

                if (v_WTA == h_WTA) { // same WTA
                    if (v_city == h_city) { // same city
                        weight_matrix[v_idx][h_idx].Gp = 0;
                        weight_flag = "0";
                    }
                    else { // in the same_WTA but btw different cities
                        weight_matrix[v_idx][h_idx].Gp = param.same_WTA_diff_cities;
                        weight_flag = "-";
                    }
                }
                else if (fabs((v_WTA)-(h_WTA)) == 1 || fabs((v_WTA)-(h_WTA)) == (num_city - 1)) { // adjacent WTA
                    if (v_city == h_city) { // adjacent WTA - same city = inhibition!
                        weight_matrix[v_idx][h_idx].Gp = param.adj_WTA_same_cities;
                        weight_flag = "/";
                    }
                    else { // THE MOST IMPORTANT PART OF THE TRAVELING SALESMAN PROBLEM
                        weight_matrix[v_idx][h_idx].Gp = param.b_value + (1 - distance / max_distance) * param.gradient;
                        weight_flag = "*";
                    }
                }
                else if (fabs((v_WTA % num_city) - (h_WTA % num_city)) != 1) { //non-adjacent WTA networks
                    if (v_city == h_city) {
                        weight_matrix[v_idx][h_idx].Gp = param.non_adj_WTA_same_cities; // inhibition btw same cities
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

    inline void tsp::SetWeightMatrixLateralBM() {
        return 0;
    }

    void tsp::wta_initialize(double* potential[], double init_val) {
        // First WTA zeroing process
        for (int i = 0; i < num_city; i++) {
            potential[side_h][i] = init_val; // init_val should be param.pt_init
        }
    }

    void tsp::wta_condition_update(sm_spk& spk_now, double tnow, double tend) {

        // Q. How to handle overlapped case? There are two obstacles to handle.
        int previous_spike_within_WTA_index = -1;
        bool wta_condition_checker = false;

        // Caution : do not use continue option! This it stands for each spike infos.
        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

            int h_WTA = it->second / num_city + 1;
            int h_city = it->second % num_city + 1;
            wta_condition_checker = false;

            // Reset event & last_spk_st update should be avoided . Should I add WTA[i][j].iso == true condition?
            if (!(spk_now.st || spk_now.reset)) {

                // Dummy event of the end of refractory time
                if (spk_now.iso == -1) { //WTA[h_WTA][h_city].route == true
                    WTA[h_WTA][h_city].route = false;
                    // printf("OFF_WTA[%d][%d]", h_WTA, h_city);
                    if (param.hw_CAP_ISO) {
                        cap_iso_update();
                    }
                }

                // Real spike event
                else if (spk_now.iso == 1) { //WTA[h_WTA][h_city].route == false
                    // First, check whether if there was a spike from the same WTA or other WTA of the same city.
                    // Be aware of the spike itself.
                    /*
                    for (int i = 1; i < num_city; i++) {

                        if (tnow - last_spk_st[side_h][(i - 1) * num_city + h_city-1] < param.refractory_time && it->second != (i - 1) * num_city + h_city-1) {
                            wta_condition_checker = true;
                        }
                        if (tnow - last_spk_st[side_h][(h_WTA - 1) * num_city + i-1] < param.refractory_time && it->second != (h_WTA - 1) * num_city + i-1) {
                            wta_condition_checker = true;
                        }
                    }
                    */

                    for (int i = 1; i < num_city + 1; i++) {
                        if (WTA[h_WTA][i].route == true || WTA[i][h_city].route == true) {
                            wta_condition_checker = true;
                        }
                    }

                    if (wta_condition_checker == false) {
                        WTA[h_WTA][h_city].route = true;
                        if (tnow > (tend - 1) && tnow < tend) {
                            spike_counter[h_WTA][h_city].num_spike++;
                            last_spike[h_WTA] = h_city;
                        }
                        // printf("ON_WTA[%d][%d]", h_WTA, h_city);
                        //previous_spike_within_WTA_index = h_WTA;
                        if (param.hw_CAP_ISO) {
                            cap_iso_update();
                        }
                    }
                    else { // WTA already working! spike won't be functioning. Just pass by
                       //printf("_________WTA error!________Error on WTA[%d][%d]", h_WTA, h_city);
                    }
                }
                // until here
            }

            if (it->first == side_v) {
                /*
                // Clear out all the route and this city is owned exclusively
                for (int i = 1; i < num_city + 1; i++) {
                    WTA[i][h_city].route = false;
                    WTA[h_WTA][i].route = false;
                }
                */

                // WTA[h_WTA][h_city].route = true;
            }
            else { //side_h
                /*
                // Refractory Time ended -> route to false!
                // was if ((tnow - last_spk_st[it->first][it->second]) > param.refractory_time+ FLOAT_EPSILON_TIME)
                if(spk_now.iso){
                    // Turn back to the original state (transition state)
                    WTA[h_WTA][h_city].route = false;
                }

                // Newly generated spike
                //fabs(last_spk_st[it->first][it->second] - tnow) <= FLOAT_EPSILON_TIME
                else {

                    // Clear out all the route and this city is owned exclusively
                    for (int i = 1; i < num_city + 1; i++) {
                        WTA[i][h_city].route = false;
                        WTA[h_WTA][i].route = false;
                    }


                    WTA[h_WTA][h_city].route = true;
                }
                */
            }
        }
    }

    void tsp::cap_iso_update() { // This function works only when (param.hw_CAP_ISO) is true

        // Initialization of cap_iso profile
        for (int i = 1; i < num_city + 1; i++) {
            for (int j = 1; j < num_city + 1; j++) {
                WTA[i][j].iso = false;
            }
        }

        // Conduct cap_iso profile based on WTA.route
        for (int i = 1; i < num_city + 1; i++) {
            for (int j = 1; j < num_city + 1; j++) {
                if (WTA[i][j].route == true) {
                    for (int k = 1; k < num_city + 1; k++) {
                        WTA[i][k].iso = true;
                        WTA[k][j].iso = true;
                    }
                }
            }
        }

        /*
        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
            int h_WTA = it->second / num_city + 1;
            int h_city = it->second % num_city + 1;

            if (!param.hw_CAP_ISO) continue;

            // Spike event : iso on process proceed
            if (spk_now.iso) {
                for (int i = 1; i < num_city + 1; i++) {
                    // Same city in other WTA network CAP_ISO
                    WTA[i][h_city].iso = true;
                    // Other city in the same WTA network CAP_ISO
                    WTA[h_WTA][i].iso = true;
                }
            }
            // Refractory time end event : iso is turning off process proceed
            else {
                for (int i = 1; i < num_city + 1; i++) {
                    // Same city in other WTA network CAP_ISO
                    WTA[i][h_city].iso = false;
                    // Other city in the same WTA network CAP_ISO
                    WTA[h_WTA][i].iso = false;
                }
            }
        }

        */
    }

    void tsp::export_performance_info_to_csv(tsp& tsp, ofstream& exportFile, double tnow) {

        total_distance = 0;
        performance = 0;
        bool distance_defined = true;
        int arr[26 + 1] = {};
        int multiply_salesman_route = 1;

        for (int i = 1; i < num_city + 1; i++) {

            // Tour check
            int salesman_route = last_spike[i];
            if (arr[salesman_route] == 0) {
                arr[salesman_route] = 1;
            }
            else {
                distance_defined = false;
            }

            // Tour distance calculation
            int a, b;
            if (i == num_city) { // CASE 1: end city to city 1
                a = num_city;
                b = 1;
            }
            else { // CASE 2: normal case
                a = i + 1;
                b = i;
            }
            if (last_spike[a] != 0 && last_spike[b] != 0) {
                total_distance += distance_matrix[last_spike[a] - 1][last_spike[b] - 1];
            }
            else {
                distance_defined = false;
            }

            if (distance_defined == false) {
                break;
            }

        }

        for (int i = 1; i < num_city + 1; i++) {
            multiply_salesman_route = multiply_salesman_route * arr[i];
        }

        if (distance_defined || (multiply_salesman_route == 0)) {
            performance = (answer / total_distance);
            if (performance > 1) {
                performance = 0;
            }
        }
        else {
            performance = 0;
        }

        if (INIT_PERFORMANCE != performance) {
            exportFile << tnow << "," << performance << "\n";
            INIT_PERFORMANCE = performance;
        }

    }

    void tsp::export_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tnow, double tend) {

        // [FILE EXPORT] Export spike time and neuron index to csv file

        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

            int h_WTA = it->second / num_city + 1;
            int h_city = it->second % num_city + 1;

            if (it->first == side_h && h_WTA == 2) {
                exportFile << spk_now.time << "," << h_city << "\n";
            }
        }
    }

    void tsp::export_wta_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

        // [FILE EXPORT] Export spike time and neuron index in specific wta module to csv file
        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
            exportFile << spk_now.time << "," << it->first << "," << it->second + 1 << "\n";
        }
    }

    void tsp::export_num_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

        // [FILE EXPORT] Export spike time and neuron index to csv file
        for (int i = 1; i < num_city + 1; i++) { // i : neuron_index
            exportFile << i << ",";
            for (int j = 1; j < num_city + 1; j++) { // j : WTA #
                exportFile << spike_counter[j][i].num_spike << ",";
            }
            exportFile << "\n";
        }
    }

    void tsp::export_potential_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

        // [FILE EXPORT] Export neuron potentials to csv file
        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
            exportFile << spk_now.time << ",";
            for (int i = 0; i < num_neurons[side_h]; i++) {
                exportFile << potential[side_h][i] << ",";
            }
            exportFile << "\n";
        }
    }

    /*
    void core::export_last_spk_st_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
            int h_WTA = it->second / num_city + 1;
            int h_city = it->second % num_city + 1;
            exportFile << spk_now.time << ",";

            for (int i = 0; i < num_neurons[side_h]; i++) {
                exportFile << last_spk_st[side_h][i] << ",";
            }
    */
    void tsp::export_travel_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

        bool travel_flag = false;

        // [FILE EXPORT] Export travel sequence to csv file
        for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

            exportFile << spk_now.time << ",";

            for (int i = 1; i < num_city + 1; i++) {
                for (int j = 1; j < num_city + 1; j++) {
                    if (WTA[i][j].route == true) {
                        exportFile << j << ",";
                        travel_flag = true;
                    }
                }

                // No city involved in the step 'i', then export 0 as default
                if (!travel_flag) {
                    exportFile << 0 << ",";
                }

                travel_flag = false;
            }
            exportFile << "\n";
        }
    }
}
