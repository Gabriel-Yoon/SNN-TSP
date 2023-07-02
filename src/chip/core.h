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

#include "fs.h"
#include "np.h"
#include "py.h"

#include "spk.h"
#include "neuron.h"
#include "synapse.h"
#include "param.h"


struct real_data {
	double delta_g;
	double standard_g;
};

//**************************************************************************************************************//
class core
{
	private: param params;
	std::ofstream os;
	/*---------------------fields-----------------*/
	private: int num_neurons[2];
	private: std::vector<struct real_data> g_potentiation;
	private: std::vector<struct real_data> g_depression;
	private: std::vector<double> g_differ;
	private: std::vector<std::vector<synapse>> synapses;
	private: std::vector<std::vector<neuron>> layers;

	priority_queue<pair<double, spk*>, vector<pair<double, spk*>>, 
                                    spk_cmp> queue_ext;
    priority_queue<pair<double, spk*>, vector<pair<double, spk*>>, 
                                    spk_cmp> queue_spk;
    priority_queue<pair<double, spk*>, vector<pair<double, spk*>>, 
                                    spk_cmp> queue_wup_ext;
    priority_queue<pair<double, spk*>, vector<pair<double, spk*>>, 
                                    spk_cmp> queue_wup_spk;

	private: std::string export_ptn_file[2];
	/*---------------------methods----------------*/
	// core constructor
	public: core(const char* param_file);
	public: double run();
	public: void print_params();
	public: void initialize(char* fextspk, char* fexttime, char* fwij, char* fwij_gp, char* fwij_gm);

    private: template<int is_spk, int is_rng>
    void run_loop(double tnow, double tpre, spk &spk_now, int which_spk, double &simtick, int &new_spk);	

	private: void weight_load(int cell_type, std::string fweight);
	private: void weight_save(int cell_type, std::string filename);
	private: void ext_spike_load(double tend);

	int compare_threshold(double tnow);
	private: int get_spk(spk **spk_now, int *which_spk);
    private: void ext_spike_load(char *fext, char *ftime);
};

#endif
