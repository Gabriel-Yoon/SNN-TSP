#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(param& _params) {
    _leakyTau = _params.pt_lk_tau;
	_randomWalkStep = _params.random_walk_step;
	_randomWalkStepUp = _params.random_walk_step_up;
	_randomWalkStepDown = _params.random_walk_step_down;
}
void lif_neuron::ManualSet(param& _params) {
	_memV = _params.pt_init;                        // membrane potential init to reset potential
	_Vreset = _params.pt_init;              	    // reset potential
	_Vth = _params.pt_threshold;               	    // threshold voltage
	_refractoryPeriod = _params.refractory_time;    // refractory period

	_leakyTau = _params.pt_lk_tau;
	_randomWalkStep = _params.random_walk_step;
	_randomWalkStepUp = _params.random_walk_step_up;
	_randomWalkStepDown = _params.random_walk_step_down;
}
//----------------------------------------------------
double& lif_neuron::leakyTau() { // getter function
    return this->_leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_Leak(double tpre, double tnow) {
    _memV = _memV * std::exp(-(tnow - tpre)) / _leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_RandomWalk() {
	
	//bool r = py::random::uniform_binary();
	bool _randomWalkFlag = this->_rng->rng_randomwalk->get_val();
	bool _isStepUpDownSame = (_randomWalkStepUp == _randomWalkStepDown);
	/*
				  flag		
	UPDOWN     T		F
	 SAME |-----------------		
		  |	  
		T |	  +step	  -step
		  |
		F |	  +up	  -down
		  |	
	*/
	int _randomWalkSign = (_randomWalkFlag) ? 1 : -1;
	double _randomWalkValue = _randomWalkStep;
	if (!_isStepUpDownSame) {
		_randomWalkValue = (_randomWalkFlag) ? _randomWalkStepUp : _randomWalkStepDown;
	}
	_memV += _randomWalkSign * _randomWalkValue;
}
//**************************************************************************************************************//