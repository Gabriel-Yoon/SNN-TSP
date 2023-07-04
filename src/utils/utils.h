#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "py.h"
#include "../chip/neuron.h"
#include "../chip/synapse.h"
#include "../chip/spk.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100
extern int INIT_VAL = 0;

using namespace std;
using json = nlohmann::json;

//**************************************************************************************************************//

namespace utils {

    void printProgress(double start, double end) {
        double percentage = start / end;
        int val = (int)(percentage * 100);
        if (val != INIT_VAL) {
            INIT_VAL++;
            int lpad = (int)(percentage * PBWIDTH);
            int rpad = PBWIDTH - lpad;
            printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
            fflush(stdout);
        }
    }

    void saveSynapseGpGmToFile(std::string& filename, std::vector<std::vector<synapse>> _synapses){
        auto result = nlohmann::json{
        {"weight_gp", json::array()},
        {"weight_gm", json::array()},
        };

        for (auto i = 0; i < _synapses[0].size(); i++) {
            auto& _Gp = result["weight_gp"];
            auto& _Gm = result["weight_gm"];
            for (auto j = 0; j < _synapses[1].size(); j++) {
                // You'll need to convert whatever Items is into a JSON type first
                _Gp[i].push_back(_synapses[i][j].Gp);
                _Gm[i].push_back(_synapses[i][j].Gm);
            }
        }

        std::ofstream out(filename + ".json");
        out << result;
    }

    void saveNeuronMemVToFile(std::string& filename, std::vector<std::vector<neuron>> _layers, double _time){
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
    
    // IN PROGRESS
    /*
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

#endif