# include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
// Manual Setting from parameters file (JSON file)
neuron::neuron(param &_params){
    _memV = _params.pt_init;                        // membrane potential init to reset potential
	_Vreset = _params.pt_init;              	    // reset potential
    _Vth = _params.pt_threshold;               	    // threshold voltage
    _refractoryPeriod = _params.refractory_time;    // refractory period

    _lastSpkTime = -1;        // latest spike time history
    _lastSpkTimeIN_PAUSE = -1;// latest spike time history
    _lastSpkTimeST_PAUSE = -1;// latest spike time history

	_rng = new rng(_params);
}
void neuron::ManualSet(param& _params) {
    _memV = _params.pt_init;                        // membrane potential init to reset potential
    _Vreset = _params.pt_init;              	    // reset potential
    _Vth = _params.pt_threshold;               	    // threshold voltage
    _refractoryPeriod = _params.refractory_time;    // refractory period
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
void neuron::updateLastSpkTime(double _newLastSpkTime) {
    this->_lastSpkTime = _newLastSpkTime;
}
//----------------------------------------------------

//----------------------------------------------------
void neuron::memV_WhiteNoise() {
    this->_memV += _rng->rng_white_noise->get_val();
}
//----------------------------------------------------
void neuron::memV_Reset() {
    this->_memV = _Vreset;
}
//----------------------------------------------------