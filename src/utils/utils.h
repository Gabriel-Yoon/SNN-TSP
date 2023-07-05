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
#include "../chip/spk.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100

using namespace std;
using json = nlohmann::json;

//**************************************************************************************************************//

class neuron;
class synapse;

namespace utils {

    void printProgress(double start, double end);

    void callSynapseGpGm(const char* filepath, std::vector<std::vector<synapse>> _synapses);
    // void callNeuronNumbers(std::string& filepath, core* _core);
    
    void saveSynapseGpGm(std::string& filename, std::vector<std::vector<synapse>> _synapses);
    void saveNeuronMemV(std::string& filename, std::vector<std::vector<neuron>> _layers, double _time);
    
    // void saveSpikeTimeToFile(std::string& filename, spk* spike);


};

//**************************************************************************************************************//

#endif