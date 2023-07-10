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
#include "../utils/fs.h"
#include "../utils/py.h"

#include "rng.h"
#include "spike.h"
#include "neuron.h"
#include "lif_neuron.h"
#include "neuron_layer.h"
#include "synapse.h"
#include "synapse_array.h"
#include "param.h"
#include "../tsp/tsp.h"

//**************************************************************************************************************//

// Forward declarations
class tsp;


class core
{
	private: param params;
	std::ofstream os;
	/*---------------------fields-----------------*/
	typedef std::priority_queue<std::pair<double, spike*>, std::vector<std::pair<double, spike*>>, spk_cmp> SpikeMagazine;
	typedef std::priority_queue<std::pair<double, random_walk*>, std::vector<std::pair<double, random_walk*>>, random_walk_cmp> RandomWalkSchedule;
	typedef neuron_layer<lif_neuron> LIFNeuronLayer;
	typedef synapse_array<synapse> PCMSynapseArray;


	public: int _numCity;
	private: int num_neurons[2];

	private: LIFNeuronLayer visibleLayer;
	private: LIFNeuronLayer hiddenLayer;
	private: PCMSynapseArray synapseArray;

	private: SpikeMagazine visibleMagazine;
	private: SpikeMagazine hiddenMagazine;
	private: SpikeMagazine ShellCatcher; // this is for recording the spike history

	private: RandomWalkSchedule visibleRandomWalkSchedule;
	private: RandomWalkSchedule hiddenRandomWalkSchedule;

	private: SpikeMagazine queue_ext;
    private: SpikeMagazine queue_spk;
    private: SpikeMagazine queue_wup_ext;
    private: SpikeMagazine queue_wup_spk;

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

	private: std::tuple<np::array_2d<uint8_t>, np::array_2d<int8_t>> load_mnist_28(std::string dataset, np::array_1d<int> digits);
	private: void generateMagazine(double tend);
	private: void loadMagazine(const char* mag_file);			// load spikes into _visibleMagazine
	private: void setRandomWalkSchedule(double tend, int side, RandomWalkSchedule RandomWalkSchedule);	// set random walk schedule. use annealing schedule here

	private: int assignTask(spike **run_spike, double tpre, double tnow, int magazine_side);	// reload spike from either the visible or hidden magazine
	private: void shootSpike(spike& run_spike, int& phase); // shoot at other side magazine, create reset, st, dummy event at self magazine
	private: void reloadSpike(double tnow); // compare
	public: void run_simulation();

	// Potential Update
	private: void potentialUpdate(spike& run_spike);

	// Learning Method
	private: void STDP();

	private: void ext_spike_load(double tend);

	int compare_threshold(double tnow);
	private: int get_spk(spike **spk_now, int *which_spk);

};

#endif
