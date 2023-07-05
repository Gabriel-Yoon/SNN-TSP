#ifndef _CORE_H_
#define _CORE_H_

#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "../utils/np.h"
#include "../utils/py.h"

#include "spike.h"
#include "lif_neuron.h"
#include "neuron.h"
#include "synapse.h"
#include "param.h"
#include "../tsp/tsp.h"


struct real_data {
	double delta_g;
	double standard_g;
};

//**************************************************************************************************************//

// Forward declarations
class tsp;

class core
{
	private: param params;
	std::ofstream os;
	/*---------------------fields-----------------*/
	public: int _numCity;
	private: int num_neurons[2];
	private: std::vector<struct real_data> g_potentiation;
	private: std::vector<struct real_data> g_depression;
	private: std::vector<double> g_differ;
	private: std::vector<std::vector<synapse>> synapses;
	private: std::vector<std::vector<lif_neuron>> layers;

	priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, 
                                    spk_cmp> queue_ext;
    priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, 
                                    spk_cmp> queue_spk;
    priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, 
                                    spk_cmp> queue_wup_ext;
    priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, 
                                    spk_cmp> queue_wup_spk;

	private: std::string export_ptn_file[2];

	double* wsum[2];
	/*---------------------methods----------------*/
	// core constructor
	public: core(const char* param_file);
	public: double run();
	public: void print_params();
	public: void initialize();

    private: template<int is_spk, int is_rng>
    void run_loop(double tnow, double tpre, spike &spk_now, int which_spk, double &simtick, int &new_spk);	

	private: void potential_update_by_spk(spike& spk_now);
	private: template<int side> void potential_update_by_spk_core(spike& spk_now, double* wsum);
			
	private: void weight_load(int cell_type, std::string fweight);
	private: void weight_save(int cell_type, std::string filename);
	private: void ext_spike_load(double tend);

	int compare_threshold(double tnow);
	private: int get_spk(spike **spk_now, int *which_spk);

};

#endif
