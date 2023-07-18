#ifndef _SYNAPSE_ARRAY_H_
#define _SYNAPSE_ARRAY_H_

#include <vector>
#include <type_traits>

#include "synapse.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

template <typename SynapseType, std::enable_if_t<std::is_base_of<synapse, SynapseType>::value, bool> = true>
class synapse_array
{
    /*---------------------fields-----------------*/
    public: std::vector<std::vector<SynapseType>> _synapses;
    /*---------------------methods----------------*/
    public: void callSynapseArrayGpGm(const std::string& filepath) {
        std::ifstream ifs(filepath);
        json _synapseSavedFile = json::parse(ifs);

        if (_synapseSavedFile.empty()) {
            std::cout << "Synapse file error: Empty JSON file" << std::endl;
            return;
        }

        // Validate the size of _synapses and weight_gp
        if (_synapseSavedFile["weight_gp"].empty()) {
            std::cout << "Synapse file error: weight_gp array not found" << std::endl;
            return;
        }

        int num_rows = _synapseSavedFile["weight_gp"].size();
        int num_cols = _synapseSavedFile["weight_gp"][0].size();

        if (_synapses.size() != num_rows || _synapses[0].size() != num_cols) {
            std::cout << "Synapse file error: Size mismatch between _synapses and weight_gp" << std::endl;
            return;
        }

        for (int i = 0; i < num_rows; i++) {
            if (_synapseSavedFile["weight_gp"][i].size() != num_cols) {
                std::cout << "Synapse file error: weight_gp row size mismatch" << std::endl;
                return;
            }

            for (int j = 0; j < num_cols; j++) {
                // Assuming the JSON data contains valid floating-point values
                _synapses[i][j].Gp = _synapseSavedFile["weight_gp"][i][j].get<double>();
            }
        }

        std::cout << "Synapse Gp Set Done" << std::endl;

        // Validate the size of weight_gm
        if (!_synapseSavedFile["weight_gm"].empty()) {
            if (_synapseSavedFile["weight_gm"].size() != num_rows ||
                _synapseSavedFile["weight_gm"][0].size() != num_cols) {
                std::cout << "Synapse file error: Size mismatch between _synapses and weight_gm" << std::endl;
                return;
            }

            for (int i = 0; i < num_rows; i++) {
                for (int j = 0; j < num_cols; j++) {
                    // Assuming the JSON data contains valid floating-point values
                    _synapses[i][j].Gm = _synapseSavedFile["weight_gm"][i][j].get<double>();
                }
            }

            std::cout << "Synapse Gm Set Done" << std::endl;
        } else {
            // If weight_gm is not found, set Gm to 0 for all synapses
            for (int i = 0; i < num_rows; i++) {
                for (int j = 0; j < num_cols; j++) {
                    _synapses[i][j].Gm = 0.0;
                }
            }
            std::cout << "Synapse Gm set to 0 for all synapses" << std::endl;
        }
    }

    //----------------------------------------------------
    public: void saveSynapseArrayGpGm(std::string& filename){
        auto result = nlohmann::json{
        {"weight_gp", nlohmann::json::array()},
        {"weight_gm", nlohmann::json::array()},
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