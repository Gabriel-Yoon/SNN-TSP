#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(const char* param_file) : neuron(param_file), params(param_file) {
    _leakyTau = params.pt_lk_tau;
}
lif_neuron::lif_neuron() {

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
    neuron::set_memV(memV() * exp_val);
}
//----------------------------------------------------
void lif_neuron::memV_RandomWalk() {
	
	//bool r = py::random::uniform_binary();
	bool _randomWalkFlag = this->rng.rng_randomwalk->get_val();
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
	this->set_memV(this->_memV + _randomWalkMul * _randomWalkStep);
}
//**************************************************************************************************************//