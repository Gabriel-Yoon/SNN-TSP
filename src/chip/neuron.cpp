# include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
// Manual Setting from parameters file (JSON file)
neuron::neuron(param &_params){
    _memV = _params.pt_init;
	_Vreset = _params.pt_init;              	// membrane potential
    _Vth = _params.pt_threshold;               	// threshold voltage
    _refractoryPeriod = _params.refractory_time; // refractory period
	_rng = new rng(_params);
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
    double noise = _rng->rng_white_noise->get_val();
    this->set_memV(this->_memV + noise);
}
//----------------------------------------------------
void neuron::memV_Reset() {
	this->set_memV(_Vreset);
}
//----------------------------------------------------