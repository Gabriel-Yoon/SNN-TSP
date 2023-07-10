#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "np.h"
#include "py.h"
#include "utils.h"
#include "../chip/neuron.h"
#include "../chip/synapse.h"
#include "../chip/core.h"
#include "../chip/spike.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100

using namespace std;
using json = nlohmann::json;

//**************************************************************************************************************//

namespace utils {

    void printProgress(double tstart, double tend) {
        double percentage = tstart / tend;
        int val = (int)(percentage * 100);
        
        int lpad = (int)(percentage * PBWIDTH);
        int rpad = PBWIDTH - lpad;
        printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }

    /*
    void saveNeuronLayerMemV(std::string& filename, NeuronLayer _layers, double _time){
        auto result = nlohmann::json{
        {"membrane potential", json::array()}
        };

        for (auto i = 0; i < _layers[0].size(); i++) {
            auto& _memV = result["membrane potential"];
            for (auto j = 0; j < _layers[1].size(); j++) {
                // You'll need to convert whatever Items is into a JSON type first
                _memV[i].push_back(_layers[i][j]._memV);
            }
        }

        std::ofstream out(filename + ".json");
        out << result;
    }
    */
 
    
    
    // IN PROGRESS
    /*

    void callNeuronNumbers(std::string& filepath, core* _core){

        std::ifstream ifs(filepath);
        json _neuronNumSavedFile = json::parse(ifs);

        // IN PROGRESSß
    }

    void saveSpikeTimeToFile(std::string& filename, spk* spike){
        auto result = nlohmann::json{
        {"membrane ", json::array()}
        };

        for (auto it = spike->_spk.begin(); it != spike->_spk.end(); it++) {
            auto& _memV = result["membrane potential"];
            _memV[i].push_back(spike->_spk);
        }

        std::ofstream out(filename + ".json");
        out << result;
    }
    */
};

//**************************************************************************************************************//