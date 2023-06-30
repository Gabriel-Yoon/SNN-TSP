#ifndef __TSP_H__
#define __TSP_H__

#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <fstream>  //ofstream
#include <stdio.h>

#include "synapse.hpp"
#include "simulation_parameters.hpp"
#include "core.hpp"

using namespace std;

struct spike_count {
	int num_spike;
};

struct WTA_condition {
	bool route;
	bool iso;
};

enum : int {
	lateral_BM,
	bilateral_BM
};

/* TSP parameter definitions and functions*/ 
class tsp {
    private:
	    // CSP parameters
	    int num_city; 
	    double answer; // 19 for 5 cities, 937 for 26 cities.
	    double total_distance;
	    double performance; // later change it to (time,performance) format
		int mode;

	    // Neurons
		int num_neurons[2];
		int num_neurons_bias[2];

        // Weight
        vector<vector<double>> distance_matrix;
		
    	// Weight Setup
    	double same_WTA_diff_cities;
    	double adj_WTA_same_cities;
    	double non_adj_WTA_same_cities;
    	double b_value;
    	double gradient;

        // WTA condition fulfillment parameters
    	vector<vector<struct WTA_condition>> WTA; // traveling order marking array
	    vector<vector<struct spike_count>> spike_counter; // spike counting array
	    vector<double> last_spike;
	    vector<bool> WTA_at_least_one_spike; // instantaneous firing probability checking array
	
	public:
		tsp(); // later change to tsp(std::string tsp_param_file)
		void neuron_setup();
		void weight_setup_lateral_BM();
		void weight_setup_bilateral_BM();

		// CSP function
		void wta_initialize(double* potential[], double init_val);
		void wta_condition_update(sm_spk& spk_now, double tnow, double tend);
		void cap_iso_update();
	
		// Export method
		void export_spike_info_to_csv(tsp& tsp,ofstream& exportFile, sm_spk& spk_now, double tnow, double tend);
		void export_num_spike_info_to_csv(tsp& tsp,ofstream& exportFile, sm_spk& spk_now, double tend);
		void export_wta_spike_info_to_csv(tsp& tsp,ofstream& exportFile, sm_spk& spk_now, double tend);
		void export_potential_info_to_csv(tsp& tsp,ofstream& exportFile, sm_spk& spk_now, double tend);
		void export_travel_info_to_csv(tsp& tsp,ofstream& exportFile, sm_spk& spk_now, double tend);
		void export_performance_info_to_csv(tsp& tsp,ofstream& exportFile, double tnow);

};


#endif