#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
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
typedef priority_queue<pair<double, spike*>, vector<pair<double, spike*>>, spk_cmp> SpikeQueue;
typedef std::vector<std::vector<synapse>> Synapse;
typedef std::vector<std::vector<neuron>> NeuronLayer;
typedef std::vector<std::vector<lif_neuron>> LIFNeuronLayer;

class neuron;
class synapse;

namespace utils {

    void printProgress(double start, double end);

    void callSynapseGpGm(const char* filepath, Synapse _synapses);
    // void callNeuronNumbers(std::string& filepath, core* _core);
    
    void saveSynapseGpGm(std::string& filename, Synapse _synapses);
    void saveNeuronMemV(std::string& filename, NeuronLayer _layers, double _time);
    void saveSpikeQueue(std::string& filename, SpikeQueue _spikeQueue);
    
    // void saveSpikeTimeToFile(std::string& filename, spk* spike);


};

//**************************************************************************************************************//

#endif