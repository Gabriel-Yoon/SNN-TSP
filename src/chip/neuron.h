
#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

#include "../utils/utils.h"
#include "param.h"
#include "rng.h"

// Forward declarations.


// neuron ฐüทร
class neuron
{ // storing neuron potential. Be aware that there is no position variable
    friend class core;
    friend class synapse;

    private: param params;
    private: rng rng;
    /*---------------------fields-----------------*/
    private: double _memV;              // membrane potential
    private: double _Vth;               // threshold voltage
    private: double _lastSpkTime;       // latest spike time history
    private: double _refractoryPeriod;  // refractory period
    /*---------------------methods----------------*/    
    /// LIF neuron Class constructor.
    public: neuron(const char* param_file);
    /// LIF neuron Class destructor.
    public: ~neuron();
    
    public: double& memV();                                 // Gets the membrane potential
    public: double& Vth();                                  // Gets the threshold voltage
    public: double& lastSpkTime();                          // Gets the latest spike time history
    public: double& refractoryPeriod();                     // Gets the refractory period
    public: void set_memV(double _newV);                    // Sets the membrane potential to new value
    public: void updateLastSpkTime(double _newLastSpkTime); // Sets the last spike time to latest spike time
    public: void update_memV_by_random_walk();              // Update membrane potential by random walk
    public: void update_memV_by_white_noise();              // Update membrane potential by white noise
};

#endif /* _NEURON_H_ */
