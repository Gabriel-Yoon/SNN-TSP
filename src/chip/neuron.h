
#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "../utils/utils.h"
#include "param.h"
#include "rng.h"

// Forward declarations.
class rng;

//**************************************************************************************************************//
class neuron
{ // storing neuron potential. Be aware that there is no position variable
    friend class core;
    friend class synapse;

    /*---------------------fields-----------------*/
    public: double _memV;               // membrane potential
    public: double _Vth;                // threshold voltage
    public: double _Vreset;             // reset voltage
    public: double _refractoryPeriod;   // refractory period
    public: double _lastSpkTime;        // latest spike time history
    public: double* _lastSpkTimeIN_PAUSE;// latest spike time history
    public: double* _lastSpkTimeST_PAUSE;// latest spike time history
    private: rng* _rng;
    /*---------------------methods----------------*/    
    /// LIF neuron Class constructor.
    public: neuron();
    public: neuron(param &_param);           // Manually set parameters from params
    public: ~neuron();

    public: double& memV();                                 // Gets the membrane potential
    public: double& Vth();                                  // Gets the threshold voltage
    public: double& lastSpkTime();                          // Gets the latest spike time history
    public: double& refractoryPeriod();                     // Gets the refractory period
    
    public: void set_memV(double _newV);                    // Sets the membrane potential to new value
    public: void updateLastSpkTime(double _newLastSpkTime); // Sets the last spike time to latest spike time
    public: void memV_WhiteNoise();              // Update membrane potential by white noise
    public: void memV_Reset();                   // Reset memV
};

// Constructors
inline neuron::neuron() : _memV(0.0),_Vth(1.0),_Vreset(0.0),_lastSpkTime(0),_lastSpkTimeIN_PAUSE(0),_lastSpkTimeST_PAUSE(0),_refractoryPeriod(4e-3)
{}
//----------------------------------------------------
// Destructor
inline neuron::~neuron(){
	if(_rng != nullptr)
		::free(_rng);
}
//**************************************************************************************************************//
#endif /* _NEURON_H_ */
