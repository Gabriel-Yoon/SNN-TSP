#ifndef _PARAM_H_
#define _PARAM_H_

#include <string>
#include <vector>

#define FLOAT_EPSILON_TIME 1e-10

using namespace std;
//**************************************************************************************************************//
// Simulation parameter definitions
struct param {

	double timestep;						// Timescale for external spikes
	double timestep_rng;					// Timescale for random walk event
	
	int steps_transition;					// Num of time steps for transition between 2 phases
	int steps_data;							// Num of time steps for data phase per 1 pattern
	int steps_model;						// Num of time steps for model phase per 1 pattern

	int num_of_label;

	double wlr_width;						// WLR(word line) width
	double refractory_time;					// Refractory time

	double delay_spikein2out;				// Spike in to out delay (bipolar & Gp/Gm)
	double delay_spikeout2wlr;				// Spike out to wlr delay (bipolar)


	double delay_spikeout2wlr_data_v;		// Spike out to wlr delay in data phase (Gp/Gm)
	double delay_spikeout2wlr_data_h;
	double delay_spikeout2wlr_model_v;		// Spike out to wlr delay in model phase (Gp/Gm)
	double delay_spikeout2wlr_model_h;

	double delay_spikeout2wup;				// Spike out to weight update delay (bipolar)
	double delay_spikeout2wup_data;			// Spike out to weight update delay in data phase (Gp/Gm)
	double delay_spikeout2wup_model;		// Spike out to weight update delay in model phase (Gp/Gm)
	double delay_spikeout2td3;				// Spike out to TD3 delay (Gp/Gm)

	double tset_width;						// Set pulse width (Gp/Gm)
	double treset_width;					// Reset pulse width (Gp/Gm)

	double pt_init;							// Initial and reset potential vlaue
	double pt_alpha;						// Alpha parameter for potential update by spike

	double pt_lk_tau;						// Time constant for leakage
	double pt_threshold;

	double wt_delta_g_set;					// Weight update set step size (Gp/Gm)
	double wt_delta_g_reset;				// Weight update reset step size (Gp/Gm)

	double max_weight;						// Max weight (bipolar & Gp/Gm)
	double min_weight;						// Min weight (bipolar & Gp/Gm)

	double random_walk_step;				// Random walk step size
	double random_walk_step_up;             // Random walk step Up size
	double random_walk_step_down;           // Random walk step Dn size

	double sigmoid_mid;                     // Random walk -> sigmoid (mid)
	double sigmoid_temp;                    // Random walk -> sigmoid (temp)

	int neurons_visible_data;
	int neurons_visible_label;
	int neurons_visible_bias;

	int neurons_hidden_data;
	int neurons_hidden_bias;

	double wij_init_mu;
	double wij_init_sigma;

	int num_of_image_learning;
	int num_of_image_inference;
	int epoch_from;
	int epoch_to;

	bool enable_learning;
	bool enable_gpgm;
	bool enable_random_walk;
	bool enable_white_noise;                // white noise for random walk
	bool enable_sigmoid;                    // sigmoid function for random walk

	bool enable_sns;
	bool enable_s2m;
	double s2m_probability;
	bool enable_ps2;
	double ps2_probability;

	bool is_real_data;						// True = real data of device
	bool is_synaptic_delay;
	bool is_neuron_delay;

	std::string delay_method;
	bool inference_by_label;
	double blocked_area;

	double synapse_noise_sigma;
	double synaptic_delay_mu;
	double synaptic_delay_sigma;
	double neuron_delay_mu;
	double neuron_delay_sigma;
	double neuron_delay_exp_mean;

	int num_of_synapse;
	int conductance_level;
	double stdp_window_bipolar;

	double mnist_binary_threshold;
	double spike_rate;
	double bias_rate_factor;
	double bias_rate_factor_v;
	double bias_rate_factor_h;

    bool hw_ISO_MOD;
    bool hw_RES_EN;
    bool hw_RES_BLK;

    param(const char* param_file);
    void print_parameters();

};

enum : int {
    spk_type_both,
    spk_type_ext,
    spk_type_int,
};
//**************************************************************************************************************//
#endif