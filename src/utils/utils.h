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

namespace utils {

    void printProgress(double start, double end);
    
    // void saveSpikeTimeToFile(std::string& filename, spk* spike);


};

//**************************************************************************************************************//

#endif