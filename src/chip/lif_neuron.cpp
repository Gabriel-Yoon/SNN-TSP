#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(param &_params)
{
	this->_leakyTau = _params.pt_lk_tau;
}
void lif_neuron::ManualSet(param &_params)
{
	std::cout << "Maneual Setting LIF Neurons" << std::endl;
	this->_memV = _params.pt_init; // membrane potential init to reset potential
	// this->_Vreset = _params.pt_init;				   // reset potential
	this->_Vth = _params.pt_threshold;				   // threshold voltage
	this->_refractoryPeriod = _params.refractory_time; // refractory period

	this->_leakyTau = _params.pt_lk_tau;

	this->_randomWalkStep = _params.random_walk_step;
	this->_randomWalkStepUp = _params.random_walk_step_up;
	this->_randomWalkStepDown = _params.random_walk_step_down;
}
//----------------------------------------------------
double &lif_neuron::leakyTau()
{ // getter function
	return this->_leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_Leak(double &tpre, double &tnow)
{
	this->newmemV = this->_memV * std::exp(-(tnow - tpre) / _leakyTau);
	this->_memV = newmemV;
}
//----------------------------------------------------
void lif_neuron::memV_LeakVrest(double &tpre, double &tnow)
{
	this->newmemV = this->_Vrest + (this->_memV - this->_Vrest) * std::exp(-(tnow - tpre) / _leakyTau);
	this->_memV = newmemV;
}
//----------------------------------------------------
void lif_neuron::memV_VrestAnnealing(double &tpre, double &tnow)
{
	this->_Vrest = this->_Vrest * std::exp(-(tnow - tpre) / this->_restTau);
}

//**************************************************************************************************************//