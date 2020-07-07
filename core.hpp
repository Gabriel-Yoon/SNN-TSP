#ifndef CORE
#define CORE

#include <vector>
#include <string>

#include "spike_control.hpp"
#include "simulation_parameters.hpp"


struct sm_pcmcell {
	double Gp; // bipolar only use Gp
	double Gm;
};

struct sm_pcmcell_step {
	int Gp;
	int Gm;
};

enum {
	wij_gp,
	wij_gm
};

class core {

private:

	// Simulation Parameters
	simulation_parameters param;
	vector<vector<double>> distance_matrix;

	// Neurons
	int num_city = 10;

	int neurons_visible_city;
	int neurons_visible_bias;
	int neurons_hidden_city;
	int neurons_hidden_bias;

	int num_neurons[2];
	int num_neurons_bias[2];
	
	bool WTA[11][11]; // traveling order marking array

	double* potential[2];
	double* threshold[2];
	double* last_spk[2];    // Used for weight_update
	double* last_spk_st[2]; // Used for SI_PAUSE in neuron
	double* last_spk_in[2]; // Used for IN_PAUSE in neuron

	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_ext;
	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> queue_spk;

	// Synapse
	vector<vector<struct sm_pcmcell>> weight_matrix;
	vector<vector<struct sm_pcmcell>> weight_matrix_ideal;

	vector<vector<struct sm_pcmcell>> max_weight;
	vector<vector<struct sm_pcmcell>> min_weight;

	/* bipolar use this for both set and reset */
	vector<vector<struct sm_pcmcell>> wt_delta_g_set;
	vector<vector<struct sm_pcmcell>> wt_delta_g_reset;

	double* wsum[2];

	// Spike Control

	list<pair<int, int>> inject_prosign;
	// the format of the inject_prosign is <WTA network number, city_idx>
	// Aware of the neuron_idx = (WTA network number) * num_city + city_idx

	priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> prosign;
	//priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, spk_cmp> spk_record_tape;


	// Random Walk

	/*
	sm_rng_1bit *rng_rwalk;
	sm_rng_1bit *rng_ps2;
	sm_rng_ureal01 rng_vth;
	sm_rng_normal *rng_wt_set; // master & circuit
	sm_rng_normal *rngwt_reset; // circuit
	sm_rng_ureal01 *rng_wt_reset_rate; // circuit
	*/


	// Functions
public:

	core();
	void initialize();
	double run();

private:
	int get_spk(sm_spk** spk_now, int* which_spk);
	void ext_spike_load(double tend);

	template<int is_spk, int is_rng> void run_loop(double tnow, double tpre, sm_spk& spk_now, int which_spk, double& simtick, int& new_spk);

	// Potential-update-related methods: implemented in sm_potential.cpp
	void potential_update_by_random_walk();
	template<int side> void potential_update_by_random_walk_core();

	void potential_update_by_leak(double tdiff);
	template<int side> void potential_update_by_leak_core(double tdiff);

	void potential_update_by_spk(sm_spk& spk_now);
	template<int side> void potential_update_by_spk_core(sm_spk& spk_now, double *wsum);

	void potential_reset(sm_spk& spk_now);
	void last_spk_st_update(sm_spk& spk_now);

	int compare_threshold(double tnow);

	// Weight-update methods

	//template<bool same_WTA, bool same_city> void weight_setup_loop(int num_city);
	void weight_setup();
	inline void weight_set_gp(int v_idx, int h_idx);


	//TSP solve

};

#endif