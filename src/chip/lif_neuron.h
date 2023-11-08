#ifndef _LIF_NEURON_H_
#define _LIF_NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "rng.h"
#include "param.h"
#include "neuron.h"

//**************************************************************************************************************//
class lif_neuron : public neuron
{
    /*---------------------fields-----------------*/
public:
    double _leakyTau; // leaky tau value for LIF neuron
    double _restTau;

private:
    double newmemV;
    /*---------------------methods----------------*/
public:
    lif_neuron();
    lif_neuron(param &_params); // LIF neuron Class constructor.
    ~lif_neuron();              // LIF neuron Class destructor.
    virtual void ManualSet(param &_params);
    double &leakyTau();                         // (outdated)Gets the leaky tau
    void memV_Leak(double &tpre, double &tnow); // Update membrane potential by leak
    void memV_LeakVrest(double &tpre, double &tnow);
    void memV_VrestAnnealing(double &tpre, double &tnow);
};

inline lif_neuron::lif_neuron() : _leakyTau(4e-4), _restTau(1e-3) {} // default : 1e-3 // default : 1e-3
inline lif_neuron::~lif_neuron() {}
//**************************************************************************************************************//
#endif