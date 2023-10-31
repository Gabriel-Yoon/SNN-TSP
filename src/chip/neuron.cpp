#include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
// Manual Setting from parameters file (JSON file)
neuron::neuron()
{
	this->_memV = 0.0;	 // first membrane potential voltage
	this->_Vth = 1.0;	 // threshold voltage
	this->_Vreset = 0.0; // reset potential		: 0.0
	this->_Vrest = 0.0;
	this->_refractoryPeriod = 4e-3; // refractory period 	: 4 ms
	this->_active = true;
	this->_WTAiso = true;

	//_rng = nullptr;
	this->_randomWalkStep = 0.06;
	this->_randomWalkStepUp = 0.06;	  // default : 0.06
	this->_randomWalkStepDown = 0.06; // default : 0.06
}
//----------------------------------------------------
neuron::~neuron()
{ // if(_rng != nullptr) ::free(_rng);
}
//----------------------------------------------------
neuron::neuron(param &_params)
{
	std::cout << "Neuron Setting" << std::endl;
	this->_memV = _params.pt_init;					   // membrane potential init to reset potential
	this->_Vreset = _params.pt_init;				   // reset potential
	this->_Vth = _params.pt_threshold;				   // threshold voltage
	this->_refractoryPeriod = _params.refractory_time; // refractory period
	this->_active = true;							   // ON
	this->_WTAiso = true;

	this->_lastSpkTime = -1;		 // latest spike time history
	this->_lastSpkTimeIN_PAUSE = -1; // latest spike time history
	this->_lastSpkTimeST_PAUSE = -1; // latest spike time history

	this->_randomWalkStep = _params.random_walk_step;
	this->_randomWalkStepUp = _params.random_walk_step_up;
	this->_randomWalkStepDown = _params.random_walk_step_down;

	//_rng = new rng(_params);
}
//----------------------------------------------------
void neuron::ManualSet(param &_params)
{
	std::cout << "Neuron Manual Set" << std::endl;
	this->_memV = _params.pt_init;					   // membrane potential init to reset potential
	this->_Vreset = _params.pt_init;				   // reset potential
	this->_Vth = _params.pt_threshold;				   // threshold voltage
	this->_refractoryPeriod = _params.refractory_time; // refractory period

	this->_randomWalkStep = _params.random_walk_step;
	this->_randomWalkStepUp = _params.random_walk_step_up;
	this->_randomWalkStepDown = _params.random_walk_step_down;

	//_rng = new rng(_params);
}
//----------------------------------------------------
void neuron::turnON()
{
	this->_active = true;
}
//----------------------------------------------------
void neuron::turnOFF()
{
	this->_active = false;
}
//----------------------------------------------------
void neuron::switchONOFF()
{
	this->_active = !this->_active;
}
//----------------------------------------------------
void neuron::WTAisoON()
{
	this->_WTAiso = true;
	add_memV(0.6);
}
//----------------------------------------------------
void neuron::WTAisoOFF()
{
	this->_WTAiso = false;
	subtract_memV(0.6);
}
//----------------------------------------------------
double &neuron::memV()
{ // getter function -> no need its public now!
	return this->_memV;
}
//----------------------------------------------------
double &neuron::Vth()
{ // getter function -> no need its public now!
	return this->_Vth;
}
//----------------------------------------------------
double &neuron::lastSpkTime()
{ // getter function -> no need its public now!
	return this->_lastSpkTime;
}
//----------------------------------------------------
double &neuron::refractoryPeriod()
{ // getter function
	return this->_refractoryPeriod;
}
//----------------------------------------------------
void neuron::set_memV(double _newV)
{ // set neuron potential to new memV
	this->_memV = _newV;
}
//----------------------------------------------------
void neuron::add_memV(double _updateV)
{ // set neuron potential to new memV
	this->_memV = this->_memV + _updateV;
}
//----------------------------------------------------
void neuron::subtract_memV(double _updateV)
{ // set neuron potential to new memV
	this->_memV = this->_memV - _updateV;
}
//----------------------------------------------------
void neuron::set_Vreset(double _newVreset)
{ // set Vreset to new Vreset
	this->_Vreset = _newVreset;
}
//----------------------------------------------------
void neuron::set_Vrest(double _newVrest)
{ // set Vreset to new Vrest
	this->_Vrest = _newVrest;
}
//----------------------------------------------------
void neuron::updateLastSpkTime(double _newLastSpkTime)
{
	this->_lastSpkTime = _newLastSpkTime;
}
//----------------------------------------------------
void neuron::memV_WhiteNoise(rng &_rng)
{
	this->_memV += _rng.rng_white_noise->get_val();
}
//----------------------------------------------------
void neuron::memV_RandomWalk(rng &_rng)
{

	// bool r = py::random::uniform_binary();
	this->_randomWalkFlag = _rng.rng_randomwalk->get_val();
	this->_isStepUpDownSame = (this->_randomWalkStepUp == this->_randomWalkStepDown);
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
	this->_randomWalkSign = (_randomWalkFlag) ? 1 : -1;
	this->_randomWalkValue = this->_randomWalkStep;
	if (!_isStepUpDownSame)
	{
		_randomWalkValue = (_randomWalkFlag) ? this->_randomWalkStepUp : this->_randomWalkStepDown;
	}

	this->_memV += _randomWalkSign * _randomWalkValue;

	// if (this->_memV + _randomWalkSign * _randomWalkValue < 1e-6)
	// {
	// 	this->_memV = 0.0;
	// }
	// else
	// {
	// 	this->_memV += _randomWalkSign * _randomWalkValue;
	// }
}
//----------------------------------------------------
void neuron::memV_Reset()
{
	// std::cout << "RESET before : " << this->_memV << std::endl;
	// this->_memV = this->_Vreset;
	double _resetValue = this->_Vreset;
	this->_memV = _resetValue;
	// std::cout << "RESET after  : " << this->_memV << std::endl;
}
//----------------------------------------------------
void neuron::randomWalkStepSizeSimulatedAnnealing(double &tpre, double &tnow)
{
	// std::cout << "before : " << this->_randomWalkStep << std::endl;
	double _randomWalkSATemp = 0.1; // higher the value, slower the random walk step size decreases
	double new_randomWalkStep = 0.06 + (this->_randomWalkStep - 0.06) * std::exp(-(tnow - tpre) / _randomWalkSATemp);
	this->_randomWalkStep = new_randomWalkStep;
	// std::cout << this->_randomWalkStep << std::endl;
}
//----------------------------------------------------