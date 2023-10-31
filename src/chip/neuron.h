
#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <cstdint>
#include <utility>
#include <iostream>

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
public:
    double _memV;             // membrane potential
    double _Vth;              // threshold voltage
    double _Vreset;           // reset voltage
    double _Vrest;            // rest voltage
    double _refractoryPeriod; // refractory period
    bool _active;             // true = ON, false = OFF
    bool _WTAiso;             // in WTA, the iso should be true when a neuron in WTA module fires

    double _randomWalkStep;
    double _randomWalkStepUp;
    double _randomWalkStepDown;
    double _lastSpkTime;         // latest spike time history
    double _lastSpkTimeIN_PAUSE; // latest spike time history
    double _lastSpkTimeST_PAUSE; // latest spike time history

private:
    bool _randomWalkFlag;
    bool _isStepUpDownSame;
    int _randomWalkSign;
    double _randomWalkValue;
    /*---------------------methods----------------*/
    /// LIF neuron Class constructor.
public:
    neuron();
    neuron(param &_params); // Manually set parameters from params
    ~neuron();
    virtual void ManualSet(param &_params);

    void turnON();
    void turnOFF();
    void switchONOFF();
    void WTAisoON();
    void WTAisoOFF();

    double &memV();             // Gets the membrane potential
    double &Vth();              // Gets the threshold voltage
    double &lastSpkTime();      // Gets the latest spike time history
    double &refractoryPeriod(); // Gets the refractory period

    void set_memV(double _newV); // Sets the membrane potential to new value
    void add_memV(double _updateV);
    void subtract_memV(double _updateV);
    void set_Vreset(double _newVreset);
    void set_Vrest(double _newVrest);
    void updateLastSpkTime(double _newLastSpkTime);                        // Sets the last spike time to latest spike time
    void memV_WhiteNoise(rng &_rng);                                       // Update membrane potential by white noise
    void memV_RandomWalk(rng &_rng);                                       // Update membrane potential by random walk
    void memV_Reset();                                                     // Reset memV
    void randomWalkStepSizeSimulatedAnnealing(double &tpre, double &tnow); // Random Walk Step Size reduce by exponential function
};
//**************************************************************************************************************//
#endif /* _NEURON_H_ */
