#include <iostream>
#include <fstream>
#include <string>

#include "synapse.hpp"
#include "core.hpp"
#include "simulation_parameters.hpp"

extern double INIT_PERFORMANCE = 0;

using namespace std;

void core::export_performance_info_to_csv(ofstream& exportFile, double tnow) {

    total_distance = 0;
    performance = 0;
    bool distance_defined = true;
    int arr[26+1] = {};
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

void core::export_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tnow, double tend) {

    // [FILE EXPORT] Export spike time and neuron index to csv file

    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        if (it->first == side_h && h_WTA == 2) {
            exportFile << spk_now.time << "," << h_city << "\n";
        }
    }
}

void core::export_wta_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {

    // [FILE EXPORT] Export spike time and neuron index in specific wta module to csv file
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        exportFile << spk_now.time << "," << it->first << "," << it->second + 1 << "\n";
    }
}

void core::export_num_spike_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {

    // [FILE EXPORT] Export spike time and neuron index to csv file
    for (int i = 1; i < num_city + 1; i++) { // i : neuron_index
        exportFile << i << ",";
        for (int j = 1; j < num_city + 1; j++) { // j : WTA #
            exportFile << spike_counter[j][i].num_spike << ",";
        }
        exportFile << "\n";
    }
}

void core::export_potential_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {

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
void core::export_travel_info_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {

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