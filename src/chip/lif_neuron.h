
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
    private: param params;
	/*---------------------fields-----------------*/
	private: double _leakyTau;          // leaky tau value for LIF neuron

	/*---------------------methods----------------*/
    public: lif_neuron(const char* param_file);                 // LIF neuron Class constructor.
    public: ~lif_neuron();                                      // LIF neuron Class destructor.
    public: double& leakyTau();                                 // Gets the leaky tau
    public: void update_memV_by_leak(double tpre, double tnow); // Update membrane potential by leak
};
#endif
//**************************************************************************************************************//