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
#include "phase.h"
#include "../tsp/tsp.h"

//**************************************************************************************************************//

// Forward declarations
class tsp;


class core
{
	private: param params;
	private: rng _rng;
	std::ofstream os;
	/*---------------------fields-----------------*/
	typedef std::priority_queue<std::pair<double, spike*>, std::vector<std::pair<double, spike*>>, spk_cmp> SpikeMagazine;
	typedef std::priority_queue<std::pair<double, random_walk*>, std::vector<std::pair<double, random_walk*>>, random_walk_cmp> RandomWalkSchedule;
	typedef neuron_layer<lif_neuron> LIFNeuronLayer;
	typedef synapse_array<synapse> PCMSynapseArray;
	typedef std::vector<std::pair<double, int>> ShellCatcher;


	public: int _numCity;
	public: int _solutionDistance;
	public: std::vector<int> _optimalItinerary;
	public: std::vector<int> _optimalItineraryNeuronNum;

	private: int num_neurons[2];
	private: int num_neurons_principal[2];
	private: int num_neurons_bias[2];

	private: LIFNeuronLayer visibleLayer;
	private: LIFNeuronLayer hiddenLayer;
	private: PCMSynapseArray synapseArray;

	private: SpikeMagazine visibleMagazine;
	private: SpikeMagazine hiddenMagazine;
	
	private: RandomWalkSchedule visibleRandomWalkSchedule;
	private: RandomWalkSchedule hiddenRandomWalkSchedule;

	private: ShellCatcher visibleShellCatcher; // <time, neuron_index>
	private: ShellCatcher hiddenShellCatcher; // <time, neuron_index>

	//private: SpikeMagazine ShellCatcher; // this is for recording the spike history
	private: ShellCatcher spikeRecorder; // <side, neuron_index>
	private: std::string potentialFilePath[2];

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
	public: core(const char* param_file, const std::string& tsp_data_file_path);
	public: void initialize();
	public: void print_params();
	public: void run_simulation();
	// public: void initialize_export();

	private: std::tuple<np::array_2d<uint8_t>, np::array_2d<int8_t>> load_mnist_28(std::string dataset, np::array_1d<int> digits);

	private: void generateMagazine(double tend);
	private: void loadMagazine(const char* mag_file);			// load spikes into _visibleMagazine
	private: void setRandomWalkSchedule(double tend, int side);	// set random walk schedule. use annealing schedule here

	private: int assignTask(spike **run_spike, double& tpre, double& tnow, double& tend, int* magazine_side);	// reload spike from either the visible or hidden magazine
	private: void shootSpike(spike& run_spike, int& phase); // shoot at other side magazine, create reset, st, dummy event at self magazine
	private: void reloadSpike(double tnow); // compare
	private: void eraseTask(int& task_id);
	
	// Export Data
	private: void makeSpikeFile(std::string filename);
	private: void writeSpikeIntoFile(spike& run_spike);
	private: void exportSpikeRecorder();
	private: void exportSpikeHistoryToJson(const std::string& filename);
	private: void exportNeuronPotentialToJson(double& tnow);
	private: void exportSynapseWeightsToJson(const std::string& filename, double tnow);

	// Potential Update
	private: void potentialUpdate(spike& run_spike);

	// Learning Method
	private: void STDP(spike& run_spike, int& phase);
};

#endif
