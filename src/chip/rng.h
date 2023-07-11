#ifndef _RNG_H_
#define _RNG_H_

#include "../utils/np.h"
#include "../utils/py.h"
#include "param.h"

struct random_walk{
    double _time;
    typedef std::pair<int, double> randomwalkPosition;
	randomwalkPosition _randomWalk; // pair<side, step_size>.

	
	//bool operator < (const random_walk& r) const{
	//	return _time < r._time;
	//}
	
};

struct random_walk_cmp {
    bool operator()(std::pair<double, random_walk*> a, std::pair<double, random_walk*> b) {
        if(a.first == b.first) return a.second->_randomWalk.first < b.second->_randomWalk.first;
        return a.first > b.first;
    }
};

//**************************************************************************************************************/
class rng
{
	friend class core;
	friend class neuron;
	friend class synapse;
	/*---------------------fields-----------------*/
	public: py::rng_binomial* rng_randomwalk;						// 0 or 1
	public: py::rng_normal* rng_white_noise;
	public: py::rng_binomial* rng_s2m;								// 0 or 1
	public: py::rng_binomial* rng_ps2;								// 0 or 1
	public: py::rng_randrange* rng_randrange;						// similar to python's randrange
	public: py::rng_ureal* rng_ureal;								// 0 ~ 1
	public: py::rng_normal* rng_weight_normal;						// normal distribution (weight)
	public: py::rng_normal* rng_neuron_delay_normal;				// normal distribution (neuron_delay)
	public: py::rng_normal* rng_synaptic_delay_normal;				// normal distribution (synaptic_delay)
	// public: std::vector<double> synaptic_delay_candidate;
	public: py::rng_randrange* rng_synaptic_delay;
	public: py::rng_normal* rng_noise_normal;						// normal distribution (noise)
	public: py::rng_exponential* rng_exponential;					// exponential distribution
	// py::rng_binomial* rng_sigmoid;
	/*---------------------methods----------------*/
	/// LIF neuron Class constructor.
	public: rng(param& _params);
};
//**************************************************************************************************************//
#endif