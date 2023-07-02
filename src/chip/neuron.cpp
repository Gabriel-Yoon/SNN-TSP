# include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
neuron::neuron(const char* param_file) : params(param_file), rng(param_file) {
    _memV = params.pt_init;              // membrane potential
    _Vth = params.pt_threshold;               // threshold voltage
    _lastSpkTime = 0.0;       // latest spike time history
	_lastSpkTimeIN_PAUSE = 0.0;// latest spike time history
	_lastSpkTimeST_PAUSE = 0.0;// latest spike time history
    _refractoryPeriod = params.refractory_time;  // refractory period
}
neuron::~neuron() {

}
//----------------------------------------------------
double& neuron::memV() { // getter function
    return this->_memV;
}
//----------------------------------------------------
double& neuron::Vth() { // getter function
	return this->_Vth;
}
//----------------------------------------------------
double& neuron::lastSpkTime() { // getter function
	return this->_lastSpkTime;
}
//----------------------------------------------------
double& neuron::refractoryPeriod() { // getter function
	return this->_refractoryPeriod;
}
//----------------------------------------------------
void neuron::set_memV(double _newV) { // set neuron potential to new memV
    this->_memV = _newV;
}
//----------------------------------------------------
void neuron::updateLastSpkTime(double _newLastSpkTime) {
    this->_lastSpkTime = _newLastSpkTime;
}
//----------------------------------------------------
void neuron::memV_RandomWalk() {
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
//----------------------------------------------------
void neuron::memV_WhiteNoise() {
    double noise = this->rng.rng_white_noise->get_val();
    this->set_memV(this->_memV + noise);
}
//----------------------------------------------------
void neuron::memV_Reset() {
	this->set_memV(params.pt_init);
}
//----------------------------------------------------