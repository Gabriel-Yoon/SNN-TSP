#ifndef _SYNAPSE_ARRAY_H_
#define _SYNAPSE_ARRAY_H_

#include <vector>
#include <type_traits>

#include "synapse.h"

template <typename SynapseType, std::enable_if_t<std::is_base_of<synapse, SynapseType>::value, bool> = true>
class synapse_array
{
    /*---------------------fields-----------------*/
    public: std::vector<std::vector<synapse>> _synapses;
    /*---------------------methods----------------*/
    public: void callSynapseArrayGpGm(const char* filepath){

        std::ifstream ifs(filepath);
        json _synapseSavedFile = json::parse(ifs);

        // check the size of the _synapse and weight_gp
        bool _isSizeSame = ((_synapses.size() == _synapseSavedFile["weight_gp"].size()) 
                        && (_synapses[0].size() == _synapseSavedFile["weight_gp"][0].size()));
        
        if (_synapseSavedFile.empty() || !_isSizeSame){
            std::cout << "Synapse file error" << std::endl;
            std::cout << "SynapseArray Size : " << _synapses.size() <<" * " << _synapses[0].size() << std::endl;
        }

        if (!_synapseSavedFile["weight_gp"].empty() && _isSizeSame) {
            for (int i = 0; i < _synapses.size(); i++){
                for (int j = 0; j < _synapses[0].size(); j++){
                    _synapses[i][j].Gp = _synapseSavedFile["weight_gp"][i][j];
                }
            }
            std::cout << "Synapse Gp Set Done" << std::endl;
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
            if (_isSizeSame){
                for (int i = 0; i < _synapses.size(); i++){
                    for (int j = 0; j < _synapses[0].size(); j++){
                        _synapses[i][j].Gm = _synapseSavedFile["weight_gm"][i][j];
                    }
                }
                std::cout << "Synapse Gm Set Done" << std::endl;
            }
            else {
                std::cout << "Synapse file error" << std::endl;
            }   
        }
    }
    //----------------------------------------------------
    public: void saveSynapseArrayGpGm(std::string& filename){
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
    //----------------------------------------------------
    public: void setSynapseSize(int& N_visible, int& N_hidden){
            // Synapse Array Setting
	    _synapses.resize(N_visible);
	    for (int i = 0; i < N_visible; i++) {
		    _synapses[i].resize(N_hidden);
	    }
        std::cout << "Synapse Array Size : " << _synapses.size() <<" * " << _synapses[0].size() << std::endl;
    }
    //----------------------------------------------------
    public: void inspectWeightValues(double& min_weight, double& max_weight){
        for(int i = 0; i < _synapses.size(); i++) {
            for(int j = 0; j < _synapses[i].size(); j++) {
                if(_synapses[i][j].Gp > max_weight) {
                    _synapses[i][j].Gp = max_weight;
                } else if(_synapses[i][j].Gp < min_weight) {
                    _synapses[i][j].Gp = min_weight;
                }

                if(_synapses[i][j].Gm > max_weight) {
                    _synapses[i][j].Gm = max_weight;
                } else if(_synapses[i][j].Gm < min_weight) {
                    _synapses[i][j].Gm = min_weight;
                }
            }
        }
    }
    //----------------------------------------------------
    public: double delta_G(int& x, int& y){
        
        return _synapses[x][y].Gp - _synapses[x][y].Gm;
    }
    
};

#endif