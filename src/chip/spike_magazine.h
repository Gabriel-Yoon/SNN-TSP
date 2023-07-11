#ifndef _SPIKE_MAGAZINE_H_
#define _SPIKE_MAGAZINE_H_

#include <utility>
#include <list>
#include <vector>
#include <queue>
#include <functional>

#include "spike.h"
#include "neuron.h"
#include "synapse.h"

//----------------------------------------------------
// Forward declarations.
class spike;
class neuron;

class spike_magazine
{
    friend class spike;
    
    /*---------------------fields-----------------*/
    public: std::priority_queue<std::pair<double, spike*>, std::vector<std::pair<double, spike*>>, spk_cmp> _spikeMagazine;
    /*---------------------methods----------------*/
    // Constructor
    public: spike_magazine(){}
    // Destructor
    public: ~spike_magazine(){}
    public: void makeSpikeMagazineFile(std::string filename){
        auto result = nlohmann::json{
        {"time", json::array()},
        {"neuron", json::array()},
        };

        std::ofstream out(filename + ".json");
        out << result;
    }
    public: void writeSpikeMagazine(spike* run_spike){
        std::string filepath = "";
        std::ifstream ifs(filepath);
        json _spikesavedfile = json::parse(ifs);
        
        for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) { //
            auto& _time = _spikesavedfile["time"];
            auto& _neuron = _spikesavedfile["neuron"];
            
            _time.push_back(run_spike->_spikeTime);
            _neuron.push_back(it->second);
        }
    }
};

#endif