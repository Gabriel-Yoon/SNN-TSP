# include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
// Manual Setting from parameters file (JSON file)
neuron::neuron(){
	_memV = 0.0;
	_Vth = 1.0;
	_Vreset = 0.0;
	_refractoryPeriod = 4e-3;
	_active = true;

	//_rng = nullptr;
	_randomWalkStep = 0.06;
	_randomWalkStepUp = 0.06;
	_randomWalkStepDown = 0.06;
}
//----------------------------------------------------
neuron::~neuron(){ //if(_rng != nullptr) ::free(_rng);
}
//----------------------------------------------------
neuron::neuron(param &_params){
    std::cout << "Neuron Setting" << std::endl;
	_memV = _params.pt_init;                        // membrane potential init to reset potential
	_Vreset = _params.pt_init;              	    // reset potential
    _Vth = _params.pt_threshold;               	    // threshold voltage
    _refractoryPeriod = _params.refractory_time;    // refractory period
    _active = true;                                 // ON

    _lastSpkTime = -1;        // latest spike time history
    _lastSpkTimeIN_PAUSE = -1;// latest spike time history
    _lastSpkTimeST_PAUSE = -1;// latest spike time history

    _randomWalkStep = _params.random_walk_step;
	_randomWalkStepUp = _params.random_walk_step_up;
	_randomWalkStepDown = _params.random_walk_step_down;

	//_rng = new rng(_params);
}
//----------------------------------------------------
void neuron::ManualSet(param& _params){
	std::cout << "Neuron Manual Set" << std::endl;
	_memV = _params.pt_init;                        // membrane potential init to reset potential
    _Vreset = _params.pt_init;              	    // reset potential
    _Vth = _params.pt_threshold;               	    // threshold voltage
    _refractoryPeriod = _params.refractory_time;    // refractory period

    _randomWalkStep = _params.random_walk_step;
	_randomWalkStepUp = _params.random_walk_step_up;
	_randomWalkStepDown = _params.random_walk_step_down;

	//_rng = new rng(_params);
}
//----------------------------------------------------
void neuron::turnON(){
    this->_active = true;
}
//----------------------------------------------------
void neuron::turnOFF(){
    this->_active = false;
}
//----------------------------------------------------
void neuron::switchONOFF(){
    this->_active = !this->_active;
}
//----------------------------------------------------
double& neuron::memV() { // getter function -> no need its public now!
    return this->_memV;
}
//----------------------------------------------------
double& neuron::Vth() { // getter function -> no need its public now!
	return this->_Vth;
}
//----------------------------------------------------
double& neuron::lastSpkTime() { // getter function -> no need its public now!
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
void neuron::add_memV(double _updateV) { // set neuron potential to new memV
    this->_memV = this->_memV + _updateV;
}
//----------------------------------------------------
void neuron::updateLastSpkTime(double _newLastSpkTime) {
    this->_lastSpkTime = _newLastSpkTime;
}
//----------------------------------------------------
void neuron::memV_WhiteNoise(rng& _rng) {
    this->_memV += _rng.rng_white_noise->get_val();
}
//----------------------------------------------------
void neuron::memV_RandomWalk(rng& _rng) {
	
	//bool r = py::random::uniform_binary();
	bool _randomWalkFlag = _rng.rng_randomwalk->get_val();
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

	if (this->_memV + _randomWalkSign * _randomWalkValue < 1e-6){
		this->_memV = 0.0;
	} else {
		this->_memV += _randomWalkSign * _randomWalkValue;
	}

}
//----------------------------------------------------
void neuron::memV_Reset() {
    std::cout << "RESETTING??" << std::endl;
	this->_memV = this->_Vreset;
}
//----------------------------------------------------