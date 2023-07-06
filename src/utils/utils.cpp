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

    void saveSynapseGpGm(std::string& filename, SynapseArray _synapses){
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

    void saveNeuronMemV(std::string& filename, NeuronLayer _layers, double _time){
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
 
    void callSynapseGpGm(const char* filepath, SynapseArray _synapses){

        std::ifstream ifs(filepath);
        json _synapseSavedFile = json::parse(ifs);

        // check the size of the _synapse and weight_gp
        bool _isSizeSame = ((_synapses.size() == _synapseSavedFile["weight_gp"].size()) 
                        && (_synapses[0].size() == _synapseSavedFile["weight_gp"][0].size()));
        
        if (_synapseSavedFile.empty() || !_isSizeSame){
            std::cout << "Synapse file error" << std::endl;
        }

        if (!_synapseSavedFile["weight_gp"].empty() && _isSizeSame) {
            for (int i = 0; i < _synapses.size(); i++){
                for (int j = 0; j < _synapses[0].size(); j++){
                    _synapses[i][j].Gp = _synapseSavedFile["weight_gp"][i][j];
                }
            }
        }

        if (_synapseSavedFile["weight_gm"].empty()) {
            for (int i = 0; i < _synapses.size(); i++){
                for (int j = 0; j < _synapses[0].size(); j++){
                    _synapses[i][j].Gm = 0;
                }
            }
        } else {
            _isSizeSame = ((_synapses.size() == _synapseSavedFile["weight_gm"].size()) 
                        && (_synapses[0].size() == _synapseSavedFile["weight_gm"][0].size()));
            if (!_isSizeSame){
                std::cout << "Synapse file error" << std::endl;
            }
            else {
                for (int i = 0; i < _synapses.size(); i++){
                    for (int j = 0; j < _synapses[0].size(); j++){
                        _synapses[i][j].Gm = _synapseSavedFile["weight_gm"][i][j];
                    }
                }
            }
            
        }

   }
    
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