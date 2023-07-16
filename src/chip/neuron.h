
#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>

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
    public: bool _active;               // true = ON, false = OFF

    public: double _randomWalkStep;
    public: double _randomWalkStepUp;
    public: double _randomWalkStepDown;

    public: double _lastSpkTime;        // latest spike time history
    public: double _lastSpkTimeIN_PAUSE;// latest spike time history
    public: double _lastSpkTimeST_PAUSE;// latest spike time history
    /*---------------------methods----------------*/    
    /// LIF neuron Class constructor.
    public: neuron();
    public: neuron(param &_params);           // Manually set parameters from params
    public: ~neuron();
    public: void ManualSet(param& _params);

    public: void turnON();
    public: void turnOFF();
    public: void switchONOFF();

    public: double& memV();                                 // Gets the membrane potential
    public: double& Vth();                                  // Gets the threshold voltage
    public: double& lastSpkTime();                          // Gets the latest spike time history
    public: double& refractoryPeriod();                     // Gets the refractory period
    
    public: void set_memV(double _newV);                    // Sets the membrane potential to new value
    public: void add_memV(double _updateV);
    public: void updateLastSpkTime(double _newLastSpkTime); // Sets the last spike time to latest spike time

    public: void memV_WhiteNoise(rng& _rng);              // Update membrane potential by white noise
    public: void memV_RandomWalk(rng& _rng);              // Update membrane potential by random walk
    public: void memV_Reset();                   // Reset memV
};
//**************************************************************************************************************//
#endif /* _NEURON_H_ */
