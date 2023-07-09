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
#include "neuron.h"
#include "lif_neuron.h"
#include "neuron_layer.h"
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

	typedef priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, spk_cmp> SpikeQueue;
	
	typedef std::vector<std::pair<double, spike::spikePosition*>> ResetEventQueue; // change this to queue!
	typedef std::vector<std::pair<double, double>> RandomWalkQueue; // change this to queue!
	typedef std::vector<std::vector<synapse>> SynapseArray;
	typedef neuron_layer<neuron> NeuronLayer;
	typedef neuron_layer<lif_neuron> LIFNeuronLayer;

	private: SynapseArray _synapses;
	private: LIFNeuronLayer visible_layer;
	private: LIFNeuronLayer hidden_layer;
	private: std::vector<LIFNeuronLayer> _layers;




	private: SpikeQueue _Magazine;
	private: SpikeQueue _intMagazine;
	private: SpikeQueue _extMagazine;

	private: SpikeQueue _intVShellCatcher;
	private: SpikeQueue _intHShellCatcher;
	private: SpikeQueue _eShellCatcher;

	private: SpikeQueue queue_ext;
    private: SpikeQueue queue_spk;
    private: SpikeQueue queue_wup_ext;
    private: SpikeQueue queue_wup_spk;

	private: std::string export_ptn_file[2];

	private: double* wsum[2];

	/*
	bool export_spk_en;
    bool export_ptn_en;
    int export_spk_param[3];
    char export_ptn_param[8];
    std::vector<short> export_ptn_neurons_V;
    std::vector<short> export_ptn_neurons_h;
    std::string export_spk_file[8];
    std::string export_ptn_file[2];
    ofstream *ofs_spk_int;
    ofstream *ofs_spk_ext;
    std::string *export_file;
    ofstream *ofs_ptn_v;
    ofstream *ofs_ptn_h;
	*/
	/*---------------------methods----------------*/
	// core constructor
	public: core(const char* param_file);
	public: double run();
	public: void print_params();
	public: void initialize();
	public: void initialize_export();

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
