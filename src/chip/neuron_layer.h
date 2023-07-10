#ifndef _NEURON_LAYER_H_
#define _NEURON_LAYER_H_

#include <vector>
#include <type_traits>
#include "neuron.h"
#include "spike.h"

// SFINAE
// HOW TO USE neuron_layer<T>
// neuron_layer<T> is constructed only when the T is inherited or from the neuron class.
// neuron_layer<neuron> arr1;       -> ok
// neuron_layer<lif_neuron> arr2;   -> ok
// neuron_layer<int> arr3;          -> compile error

//**************************************************************************************************************//
//----------------------------------------------------
// Forward declarations.
class spike;
class neuron;

template <typename NeuronType, std::enable_if_t<std::is_base_of<neuron, NeuronType>::value, bool> = true>
class neuron_layer
{
    /*---------------------fields-----------------*/
    public: std::vector<NeuronType> _neurons;
    //private: int maxSize;
    /*---------------------methods----------------*/
    // Constructor
    public: neuron_layer(){}
    // Destructor
    public: ~neuron_layer(){}
    //----------------------------------------------------
    public: void ManualSet(param& _params){
        for (auto _neuron : this->_neurons){
            _neuron.ManualSet(_params);
        }
    }
    //----------------------------------------------------
    public: void addElement(const neuron& _neuron){
        if (this->_neurons.size()) {
            this->_neurons.push_back(_neuron);
        }
    }
    //----------------------------------------------------
    public: void WhiteNoise(){
        for (auto _neuron : this->_neurons){
            _neuron.memV_WhiteNoise();
        }
    }
    //----------------------------------------------------
	public: void RandomWalk(){
        for (auto _neuron : this->_neurons){
            _neuron.memV_RandomWalk();
        }
    }
    //----------------------------------------------------
    public: void Reset(){
        for (auto _neuron : this->_neurons){
            _neuron.memV_Reset();
        }
    }
    //----------------------------------------------------
	// template <typename U, std::enable_if_t<std::is_same<NeuronType, lif_neuron>::value, bool> == true>
    // public: Leak(double& tpre, double& tnow){
    //     for (auto _neuron : this->elements){
    //         _neuron.memV_Leak(tpre, tnow);
    //     }
    // }
    //----------------------------------------------------
	public: void saveNeuronLayerMemV(std::string& filename, neuron_layer _layers, double _time, int side);
    //----------------------------------------------------
    public: void writeNeuronLayerMemV(std::string& filename, neuron_layer _layers, double _time, int side);
    //----------------------------------------------------
    public: void loadSpikeToMagazine(){ // compare memV with Vth to generate spike
        
    }

};



#endif
