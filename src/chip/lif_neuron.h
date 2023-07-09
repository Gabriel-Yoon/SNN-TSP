#ifndef _LIF_NEURON_H_
#define _LIF_NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "rng.h"
#include "param.h"
#include "neuron.h"

//**************************************************************************************************************//
class lif_neuron : public neuron
{
	/*---------------------fields-----------------*/
	public: double _leakyTau;          // leaky tau value for LIF neuron

	/*---------------------methods----------------*/
    public: lif_neuron();
    public: lif_neuron(param& _params);                         // LIF neuron Class constructor.
    public: ~lif_neuron();                                      // LIF neuron Class destructor.
    public: void ManualSet(param& _params);
    public: double& leakyTau();                                 // (outdated)Gets the leaky tau
    public: void memV_Leak(double& tpre, double& tnow); // Update membrane potential by leak
};

inline lif_neuron::lif_neuron() : _leakyTau(1e-3) {}
inline lif_neuron::~lif_neuron() {}
//**************************************************************************************************************//
#endif