#ifndef _SPIKE_MAGAZINE_H_
#define _SPIKE_MAGAZINE_H_

#include <utility>
#include <list>
#include <vector>
#include <queue>
#include <functional>

#include "spike.h"
#include "neuron.h"
#include "synapse.h"

//----------------------------------------------------
// Forward declarations.
class spike;
class neuron;

class spike_magazine
{
    friend class spike;
    
    /*---------------------fields-----------------*/
    public: std::priority_queue<std::pair<double, spike*>, std::vector<std::pair<double, spike*>>, spk_cmp> _spikeMagazine;
    /*---------------------methods----------------*/
    // Constructor
    public: spike_magazine(){}
    // Destructor
    public: ~spike_magazine(){}
};

#endif