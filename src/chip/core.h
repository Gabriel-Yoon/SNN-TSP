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
private:
	param params;
	rng _rng;
	std::ofstream os;
	/*---------------------fields-----------------*/
	typedef std::priority_queue<std::pair<double, spike *>, std::vector<std::pair<double, spike *>>, spk_cmp> SpikeMagazine;
	typedef std::priority_queue<std::pair<double, random_walk *>, std::vector<std::pair<double, random_walk *>>, random_walk_cmp> RandomWalkSchedule;
	typedef neuron_layer<lif_neuron> LIFNeuronLayer;
	typedef synapse_array<synapse> PCMSynapseArray;
	typedef std::vector<std::pair<double, int>> ShellCatcher;

public:
	int _numCity;
	int _solutionDistance;
	std::vector<int> _optimalItinerary;
	std::vector<int> _optimalItineraryNeuronNum;
	std::vector<std::vector<double>> _distanceMatrix;

private:
	int num_neurons[2];
	int num_neurons_principal[2];
	int num_neurons_bias[2];
	LIFNeuronLayer visibleLayer;
	LIFNeuronLayer hiddenLayer;
	PCMSynapseArray synapseArray;
	SpikeMagazine visibleMagazine;
	SpikeMagazine hiddenMagazine;
	RandomWalkSchedule visibleRandomWalkSchedule;
	RandomWalkSchedule hiddenRandomWalkSchedule;
	ShellCatcher visibleShellCatcher; // <time, neuron_index>
	ShellCatcher hiddenShellCatcher;  // <time, neuron_index>

	// private: SpikeMagazine ShellCatcher; // this is for recording the spike history
	ShellCatcher spikeRecorder; // <side, neuron_index>
	std::string potentialFilePath[2];

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
public:
	core(const char *param_file, const std::string &tsp_data_file_path);
	void initialize();
	void print_params();
	void run_simulation();
	// public: void initialize_export();

private:
	std::tuple<np::array_2d<uint8_t>, np::array_2d<int8_t>> load_mnist_28(std::string dataset, np::array_1d<int> digits);

	void generateMagazine(double tend);
	void loadMagazine(const char *mag_file);		   // load spikes into _visibleMagazine
	void setRandomWalkSchedule(double tend, int side); // set random walk schedule. use annealing schedule here

	int assignTask(spike **run_spike, double &tpre, double &tnow, double &tend, int *magazine_side); // reload spike from either the visible or hidden magazine
	void shootSpike(spike &run_spike, int &phase);													 // shoot at other side magazine, create reset, st, dummy event at self magazine
	void reloadSpike(double tnow);																	 // compare
	void eraseTask(int &task_id);

	// Export Data
	void makeSpikeFile(std::string filename);
	void writeSpikeIntoFile(spike &run_spike);
	void exportSpikeRecorder();
	void exportSpikeHistoryToJson(const std::string &filename);
	void exportNeuronPotentialToJson(double &tnow);
	void exportSynapseWeightsToJson(const std::string &filename, double tnow);
	void exportPerformanceMostRecentSpikesToJson(const std::string &filename, double deltaTime, double tend);

	// Potential Update
	void potentialUpdate(spike &run_spike);

	// Learning Method
	void STDP(spike &run_spike, int &phase);
};

#endif
