#ifndef TSP_PARAM
#define TSP_PARAM

#include <vector>
#include <string>

using namespace std;

/* TSP parameter definitions */ 

class tsp_param {

    private:

        // Neurons
	    const int num_city; 
	    const double answer; // 19 for 5 cities, 937 for 26 cities.
	    double total_distance;
	    double performance; // later change it to (time,performance) format

	    int neurons_visible_city;
	    int neurons_visible_bias;
	    int neurons_hidden_city;
	    int neurons_hidden_bias;

        // Weight
        vector<vector<double>> distance_matrix;

        // 	
    	vector<vector<struct WTA_condition>> WTA; // traveling order marking array
	    vector<vector<struct spike_count>> spike_counter; // spike counting array
	    vector<double> last_spike;
	    vector<bool> WTA_at_least_one_spike; // instantaneous firing probability checking array


};

#endif