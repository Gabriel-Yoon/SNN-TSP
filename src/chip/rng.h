#include "../utils/np.h"
#include "../utils/py.h"
#include "param.h"
//**************************************************************************************************************/
//----------------------------------------------------
class rng
{
	private: param params;
	friend class core;
	friend class neuron;
	friend class synapse;
	/*---------------------fields-----------------*/
public :
	py::rng_binomial* rng_randomwalk;						// 0 or 1
	py::rng_normal* rng_white_noise;
	py::rng_binomial* rng_s2m;								// 0 or 1
	py::rng_binomial* rng_ps2;								// 0 or 1
	py::rng_randrange* rng_randrange;						// similar to python's randrange
	py::rng_ureal* rng_ureal;								// 0 ~ 1
	py::rng_normal* rng_weight_normal;						// normal distribution (weight)
	py::rng_normal* rng_neuron_delay_normal;				// normal distribution (neuron_delay)
	py::rng_normal* rng_synaptic_delay_normal;				// normal distribution (synaptic_delay)
	std::vector<double> synaptic_delay_candidate;
	py::rng_randrange* rng_synaptic_delay;
	py::rng_normal* rng_noise_normal;						// normal distribution (noise)
	py::rng_exponential* rng_exponential;					// exponential distribution
	// py::rng_binomial* rng_sigmoid;
	/*---------------------methods----------------*/
	/// LIF neuron Class constructor.
	public: rng(const char* param_file);
};