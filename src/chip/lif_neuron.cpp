#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(param& _params) {
    _leakyTau = _params.pt_lk_tau;
}
void lif_neuron::ManualSet(param& _params) {
	_memV = _params.pt_init;                        // membrane potential init to reset potential
	_Vreset = _params.pt_init;              	    // reset potential
	_Vth = _params.pt_threshold;               	    // threshold voltage
	_refractoryPeriod = _params.refractory_time;    // refractory period

	_leakyTau = _params.pt_lk_tau;
}
//----------------------------------------------------
double& lif_neuron::leakyTau() { // getter function
    return this->_leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_Leak(double& tpre, double& tnow) {
    double newmemV = this->_memV * std::exp(-(tnow - tpre) / _leakyTau);
	this->_memV = newmemV;
}
//----------------------------------------------------

//**************************************************************************************************************//