#include "neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
// Manual Setting from parameters file (JSON file)
neuron::neuron()
{
	this->_memV = 0.0;
	this->_Vth = 1.0;
	this->_Vreset = 0.0;
	this->_refractoryPeriod = 4e-3;
	this->_active = true;

	//_rng = nullptr;
	this->_randomWalkStep = 0.06;
	this->_randomWalkStepUp = 0.06;
	this->_randomWalkStepDown = 0.06;
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
	double _randomWalkValue = this->_randomWalkStep;
	if (!_isStepUpDownSame)
	{
		_randomWalkValue = (_randomWalkFlag) ? this->_randomWalkStepUp : this->_randomWalkStepDown;
	}

	if (this->_memV + _randomWalkSign * _randomWalkValue < 1e-6)
	{
		this->_memV = 0.0;
	}
	else
	{
		this->_memV += _randomWalkSign * _randomWalkValue;
	}
}
//----------------------------------------------------
void neuron::memV_Reset()
{
	std::cout << "RESETTING??" << std::endl;
	this->_memV = this->_Vreset;
}
//----------------------------------------------------