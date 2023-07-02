#ifndef _CORE_H_
#define _CORE_H_

#include <vector>
#include <string>

#include "spike.h"
#include "synapse.h"
#include "param.h"
#include "../utils/np.h"
#include "../utils/py.h"

struct nvmcell {
	double Gp; // bipolar only use Gp
	double Gm;
};

struct nvmcell_step {
	int Gp;
	int Gm;
};

struct custom {
    int x, y, value;
    custom(int v) : x(0), y(0), value(v) {}
};

struct cmp {
    bool operator()(const custom& a, const custom& b) {
        return a.value > b.value;
    }
};

class core {

private:

	// Simulation Parameters
	param params;

	// spk container
	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_ext;
	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_spk;
	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_wup_ext; // not used in TSP but declared
	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_wup_spk; // not used in TSP but declared

	// neurons
	int num_neurons[2];
	int num_neurons_bias[2];

	// synapse
	vector<vector<struct nvmcell>> weight_matrix;
	vector<vector<struct nvmcell>> weight_matrix_ideal;
	vector<vector<struct nvmcell>> max_weight;
	vector<vector<struct nvmcell>> min_weight;
	vector<vector<struct nvmcell>> wt_delta_g_set;	// bipolar use this for both set and reset 
	vector<vector<struct nvmcell>> wt_delta_g_reset; // bipolar use this for both set and reset 

	double* potential[2];
	double* threshold[2];
	double* last_spk[2];    // Used for weight_update
	double* last_spk_st[2]; // Used for SI_PAUSE in neuron
	double* last_spk_in[2]; // Used for IN_PAUSE in neuron
	double* wsum[2];

	// rng
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
	py::rng_binomial* rng_sigmoid;

	// Functions
public:
	core(const char* param_file);
	void initialize();
	void print_params();
	double run();

private:
	int get_spk(sm_spk** spk_now, int* which_spk);
	void ext_spike_load(double tend);

	template<int is_spk, int is_rng>
	void run_loop(double tnow, double tpre, double tend, sm_spk& spk_now, int which_spk, double& simtick, int& new_spk);

	// Potential-update-related methods: implemented in sm_potential.cpp
	void potential_update_by_random_walk(double tnow);
	void random_walk_annealing_schedule(double tnow, double tend);
	template<int side> void potential_update_by_random_walk_core(double tnow);

	void instantaneous_firing_probability();

	void potential_update_by_leak(double tdiff);
	template<int side> void potential_update_by_leak_core(double tdiff);

	void potential_update_by_spk(sm_spk& spk_now);
	template<int side> void potential_update_by_spk_core(sm_spk& spk_now, double *wsum);

	void potential_reset(sm_spk& spk_now);
	void last_spk_st_update(sm_spk& spk_now);

	int compare_threshold(double tnow);

	// Weight-update methods

	//template<bool same_WTA, bool same_city> void weight_setup_loop(int num_city);

	inline void weight_set_gp(int v_idx, int h_idx);


};

#endif