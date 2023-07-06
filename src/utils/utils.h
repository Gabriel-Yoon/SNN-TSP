#ifndef __UTILS_H__
#define __UTILS_H__

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
#include <nlohmann/json.hpp>

#include "py.h"
#include "../chip/neuron.h"
#include "../chip/synapse.h"
#include "../chip/spike.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100

using namespace std;
using json = nlohmann::json;

//**************************************************************************************************************//
class neuron;
class lif_neuron;
class synapse;

typedef priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, spk_cmp> SpikeQueue;
typedef std::vector<std::vector<synapse>> SynapseArray;
typedef std::vector<std::vector<neuron>> NeuronLayer;
typedef std::vector<std::vector<lif_neuron>> LIFNeuronLayer;

namespace utils {

    void printProgress(double start, double end);

    void callSynapseArrayGpGm(const char* filepath, SynapseArray _synapses);
    // void callNeuronNumbers(std::string& filepath, core* _core);
    
    void saveSynapseArrayGpGm(std::string& filename, SynapseArray _synapses);
    void saveNeuronLayerMemV(std::string& filename, NeuronLayer _layers, double _time);
    void writeNeuronLayerMemV(std::string& filename, NeuronLayer _layers, double _time);
    void saveSpikeQueueSpikeInfo(std::string& filename, SpikeQueue _spikeQueue);
    
    // void saveSpikeTimeToFile(std::string& filename, spk* spike);


};

//**************************************************************************************************************//

#endif