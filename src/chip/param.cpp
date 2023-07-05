#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "param.h"

using namespace std;
using json = nlohmann::json;

void param::print_parameters(){
    cout << this->pt_alpha << endl;
}

param::param(const char* param_file) {

    // param_file == "params.json"
    std::ifstream f(param_file);
    json _SNNChipParam = json::parse(f);
	
	// timestep_injection = 30e-6;
    
	timestep = _SNNChipParam["timestep"];
	timestep_rng = _SNNChipParam["timestep_rng"];

	steps_transition = _SNNChipParam["steps_transition"];
	steps_data = _SNNChipParam["steps_data"];
	steps_model = _SNNChipParam["steps_model"];

	num_of_label = _SNNChipParam["num_of_label"];

	wlr_width = _SNNChipParam["wlr_width"];
	refractory_time = _SNNChipParam["refractory_time"];

	delay_spikein2out = _SNNChipParam["delay_spikein2out"];
	delay_spikeout2wlr = _SNNChipParam["delay_spikeout2wlr"];
	delay_spikeout2wlr_data_v = _SNNChipParam["delay_spikeout2wlr_data_v"];
	delay_spikeout2wlr_data_h = _SNNChipParam["delay_spikeout2wlr_data_h"];
	delay_spikeout2wlr_model_v = _SNNChipParam["delay_spikeout2wlr_model_v"];
	delay_spikeout2wlr_model_h = _SNNChipParam["delay_spikeout2wlr_model_h"];


	delay_spikeout2wup = _SNNChipParam["delay_spikeout2wup"];
	delay_spikeout2wup_data = _SNNChipParam["delay_spikeout2wup_data"];
	delay_spikeout2wup_model = _SNNChipParam["delay_spikeout2wup_model"];
	delay_spikeout2td3 = _SNNChipParam["delay_spikeout2td3"];

	tset_width = _SNNChipParam["tset_width"];
	treset_width = _SNNChipParam["treset_width"];


	pt_init = _SNNChipParam["pt_init"];
	pt_alpha = _SNNChipParam["pt_alpha"];
	pt_threshold = _SNNChipParam["pt_threshold"];

	pt_lk_tau = _SNNChipParam["pt_lk_tau"];

	wt_delta_g_set = _SNNChipParam["wt_delta_g_set"];

	wt_delta_g_reset = _SNNChipParam["wt_delta_g_reset"];


	max_weight = _SNNChipParam["max_weight"];
	min_weight = _SNNChipParam["min_weight"];

	random_walk_step = _SNNChipParam["random_walk_step"];
	random_walk_step_up = _SNNChipParam["random_walk_step_up"];
	random_walk_step_down = _SNNChipParam["random_walk_step_down"];

	sigmoid_mid = _SNNChipParam["sigmoid_mid"];
	sigmoid_temp = _SNNChipParam["sigmoid_temp"];

	neurons_visible_data = _SNNChipParam["neurons_visible_data"];
	neurons_visible_label = _SNNChipParam["neurons_visible_label"];
	neurons_visible_bias = _SNNChipParam["neurons_visible_bias"];

	neurons_hidden_data = _SNNChipParam["neurons_hidden_data"];
	neurons_hidden_bias = _SNNChipParam["neurons_hidden_bias"];

	wij_init_mu = _SNNChipParam["wij_init_mu"];
	wij_init_sigma = _SNNChipParam["wij_init_sigma"];

	num_of_image_learning = _SNNChipParam["num_of_image_learning"];
	num_of_image_inference = _SNNChipParam["num_of_image_inference"];
	epoch_from = _SNNChipParam["epoch_from"];
	epoch_to = _SNNChipParam["epoch_to"];

	enable_learning = _SNNChipParam["enable_learning"];
	enable_random_walk = _SNNChipParam["enable_random_walk"];
	enable_white_noise = _SNNChipParam["enable_white_noise"];
	enable_sigmoid = _SNNChipParam["enable_sigmoid"];

	enable_sns = _SNNChipParam["enable_sns"];
	enable_s2m = _SNNChipParam["enable_s2m"];
	s2m_probability = _SNNChipParam["s2m_probability"];
	enable_ps2 = _SNNChipParam["enable_ps2"];
	ps2_probability = _SNNChipParam["ps2_probability"];
	is_real_data = _SNNChipParam["is_real_data"];
	is_synaptic_delay = _SNNChipParam["is_synaptic_delay"];
	is_neuron_delay = _SNNChipParam["is_neuron_delay"];
	delay_method = _SNNChipParam["delay_method"];

	inference_by_label = _SNNChipParam["inference_by_label"];
	blocked_area = _SNNChipParam["blocked_area"];

	synapse_noise_sigma = _SNNChipParam["synapse_noise_sigma"];
	synaptic_delay_mu = _SNNChipParam["synaptic_delay_mu"];
	synaptic_delay_sigma = _SNNChipParam["synaptic_delay_sigma"];
	neuron_delay_mu = _SNNChipParam["neuron_delay_mu"];
	neuron_delay_sigma = _SNNChipParam["neuron_delay_sigma"];
	neuron_delay_exp_mean = _SNNChipParam["neuron_delay_exp_mean"];

	std::string learn_method = _SNNChipParam["learn_method"];
	/*
	if (learn_method == "gpgm") {
		num_of_synapse = 2;
		enable_gpgm = true;
	}
	else if (learn_method == "bipolar") {
		num_of_synapse = 1;
		enable_gpgm = false;
	}
	*/
	conductance_level = _SNNChipParam["conductance_level"];
	stdp_window_bipolar = _SNNChipParam["stdp_window_bipolar"];

	mnist_binary_threshold = _SNNChipParam["mnist_binary_threshold"];
	spike_rate = _SNNChipParam["spike_rate"];
	bias_rate_factor = _SNNChipParam["bias_rate_factor"];
	bias_rate_factor_v = _SNNChipParam["bias_rate_factor_v"];
	bias_rate_factor_h = _SNNChipParam["bias_rate_factor_h"];

	hw_ISO_MOD = _SNNChipParam["hw_ISO_MOD"];
	hw_RES_EN = _SNNChipParam["hw_RES_EN"];
	hw_RES_BLK = _SNNChipParam["hw_RES_BLK"];
}  