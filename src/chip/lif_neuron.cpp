#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(param& _params){
    _leakyTau = _params.pt_lk_tau;
}
lif_neuron::lif_neuron() {
	_leakyTau = 1e-3;
}
lif_neuron::~lif_neuron() {

}
//----------------------------------------------------
double& lif_neuron::leakyTau() { // getter function
    return this->_leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_Leak(double tpre, double tnow) {
    double exp_val = std::exp(-(tnow - tpre) / this->_leakyTau);
	this->_memV = this->_memV * exp_val;
}
//----------------------------------------------------
void lif_neuron::memV_RandomWalk() {
	
	//bool r = py::random::uniform_binary();
	bool _randomWalkFlag = _rng->rng_randomwalk->get_val();
	bool _isStepUpDownSame = (params.random_walk_step_up == params.random_walk_step_down);
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
	int _randomWalkMul = (_randomWalkFlag) ? 1 : -1;
	double _randomWalkStep = params.random_walk_step;
	if (!_isStepUpDownSame) {
		_randomWalkStep = (_randomWalkFlag) ? params.random_walk_step_up : params.random_walk_step_down;
	}
	this->_memV = this->_memV + _randomWalkMul * _randomWalkStep;
}
//**************************************************************************************************************//