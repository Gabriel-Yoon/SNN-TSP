
#ifndef _LIF_NEURON_H_
#define _LIF_NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "../utils/py.h"
#include "param.h"
#include "neuron.h"

//**************************************************************************************************************//
class lif_neuron : public neuron
{
	/*---------------------fields-----------------*/
	public: double _leakyTau;          // leaky tau value for LIF neuron
    public: double _randomWalkStep;
    public: double _randomWalkStepUp;
    public: double _randomWalkStepDown;
	/*---------------------methods----------------*/
    public: lif_neuron();
    public: lif_neuron(param& _params);                         // LIF neuron Class constructor.
    public: ~lif_neuron();                                      // LIF neuron Class destructor.
    public: double& leakyTau();                                 // (outdated)Gets the leaky tau
    public: void memV_Leak(double tpre, double tnow); // Update membrane potential by leak
    public: void memV_RandomWalk();              // Update membrane potential by random walk
};

inline lif_neuron::lif_neuron() : _leakyTau(1e-3), _randomWalkStep(0.06), _randomWalkStepUp(0.06), _randomWalkStepDown(0.06)
{}

inline lif_neuron::~lif_neuron(){
    
}
//**************************************************************************************************************//
#endif