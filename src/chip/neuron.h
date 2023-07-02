
#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "../utils/utils.h"
#include "param.h"
#include "rng.h"

// Forward declarations.

//**************************************************************************************************************//
class neuron
{ // storing neuron potential. Be aware that there is no position variable
    friend class core;
    friend class synapse;

    private: param params;
    private: rng rng;
    /*---------------------fields-----------------*/
    public: double _memV;               // membrane potential
    public: double _Vth;                // threshold voltage
    public: double _lastSpkTime;        // latest spike time history
    public: double _lastSpkTimeIN_PAUSE;// latest spike time history
    public: double _lastSpkTimeST_PAUSE;// latest spike time history
    public: double _refractoryPeriod;   // refractory period
    /*---------------------methods----------------*/    
    /// LIF neuron Class constructor.
    public: neuron();
    public: neuron(const char* param_file);
    /// LIF neuron Class destructor.
    public: ~neuron();
    
    public: double& memV();                                 // Gets the membrane potential
    public: double& Vth();                                  // Gets the threshold voltage
    public: double& lastSpkTime();                          // Gets the latest spike time history
    public: double& refractoryPeriod();                     // Gets the refractory period
    
    public: void set_memV(double _newV);                    // Sets the membrane potential to new value
    public: void updateLastSpkTime(double _newLastSpkTime); // Sets the last spike time to latest spike time
    public: void memV_RandomWalk();              // Update membrane potential by random walk
    public: void memV_WhiteNoise();              // Update membrane potential by white noise
    public: void memV_Reset();                   // Reset memV
};
//**************************************************************************************************************//
#endif /* _NEURON_H_ */
