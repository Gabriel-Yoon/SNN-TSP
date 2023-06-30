#include <stdio.h>
#include "param.h"
#include "parson.h"


param::param(const char* param_file) {

    JSON_Value* rootValue;
    JSON_Object* rootObject;

    /* 초기화 */
    rootValue = json_parse_file(param_file);
    rootObject = json_value_get_object(rootValue);

	timestep = json_object_get_number(rootObject, "timestep");
	timestep_rng = json_object_get_number(rootObject, "timestep_rng");

	steps_transition = (int)json_object_get_number(rootObject, "steps_transition");
	steps_data = (int)json_object_get_number(rootObject, "steps_data");
	steps_model = (int)json_object_get_number(rootObject, "steps_model");

	num_of_label = (int)json_object_get_number(rootObject, "num_of_label");

	wlr_width = json_object_get_number(rootObject, "wlr_width");
	refractory_time = json_object_get_number(rootObject, "refractory_time");

	delay_spikein2out = json_object_get_number(rootObject, "delay_spikein2out");
	delay_spikeout2wlr = json_object_get_number(rootObject, "delay_spikeout2wlr");
	delay_spikeout2wlr_data_v = json_object_get_number(rootObject, "delay_spikeout2wlr_data_v");
	delay_spikeout2wlr_data_h = json_object_get_number(rootObject, "delay_spikeout2wlr_data_h");
	delay_spikeout2wlr_model_v = json_object_get_number(rootObject, "delay_spikeout2wlr_model_v");
	delay_spikeout2wlr_model_h = json_object_get_number(rootObject, "delay_spikeout2wlr_model_h");


	delay_spikeout2wup = json_object_get_number(rootObject, "delay_spikeout2wup");
	delay_spikeout2wup_data = json_object_get_number(rootObject, "delay_spikeout2wup_data");
	delay_spikeout2wup_model = json_object_get_number(rootObject, "delay_spikeout2wup_model");
	delay_spikeout2td3 = json_object_get_number(rootObject, "delay_spikeout2td3");

	tset_width = json_object_get_number(rootObject, "tset_width");
	treset_width = json_object_get_number(rootObject, "treset_width");


	pt_init = json_object_get_number(rootObject, "pt_init");
	pt_alpha = json_object_get_number(rootObject, "pt_alpha");
	pt_threshold = json_object_get_number(rootObject, "pt_threshold");

	pt_lk_tau = json_object_get_number(rootObject, "pt_lk_tau");

	wt_delta_g_set = json_object_get_number(rootObject, "wt_delta_g_set");

	wt_delta_g_reset = json_object_get_number(rootObject, "wt_delta_g_reset");


	max_weight = json_object_get_number(rootObject, "max_weight");
	min_weight = json_object_get_number(rootObject, "min_weight");

	random_walk_step = json_object_get_number(rootObject, "random_walk_step");
	random_walk_step_up = json_object_get_number(rootObject, "random_walk_step_up");               
	random_walk_step_down = json_object_get_number(rootObject, "random_walk_step_down");               

	sigmoid_mid = json_object_get_number(rootObject, "sigmoid_mid");
	sigmoid_temp = json_object_get_number(rootObject, "sigmoid_temp");

	neurons_visible_data = (int)json_object_get_number(rootObject, "neurons_visible_data");
	neurons_visible_label = (int)json_object_get_number(rootObject, "neurons_visible_label");
	neurons_visible_bias = (int)json_object_get_number(rootObject, "neurons_visible_bias");

	neurons_hidden_data = (int)json_object_get_number(rootObject, "neurons_hidden_data");
	neurons_hidden_bias = (int)json_object_get_number(rootObject, "neurons_hidden_bias");

	wij_init_mu = json_object_get_number(rootObject, "wij_init_mu");
	wij_init_sigma = json_object_get_number(rootObject, "wij_init_sigma");

	num_of_image_learning = (int)json_object_get_number(rootObject, "num_of_image_learning");
	num_of_image_inference = (int)json_object_get_number(rootObject, "num_of_image_inference");
	epoch_from = (int)json_object_get_number(rootObject, "epoch_from");
	epoch_to = (int)json_object_get_number(rootObject, "epoch_to");

	enable_learning = json_object_get_boolean(rootObject, "enable_learning");
	enable_random_walk = json_object_get_boolean(rootObject, "enable_random_walk");
	enable_white_noise = json_object_get_boolean(rootObject, "enable_white_noise");
	enable_sigmoid = json_object_get_boolean(rootObject, "enable_sigmoid");

	enable_sns = json_object_get_boolean(rootObject, "enable_sns");
	enable_s2m = json_object_get_boolean(rootObject, "enable_s2m");
	s2m_probability = json_object_get_number(rootObject, "s2m_probability");
	enable_ps2 = json_object_get_boolean(rootObject, "enable_ps2");
	ps2_probability = json_object_get_number(rootObject, "ps2_probability");
	is_real_data = json_object_get_boolean(rootObject, "is_real_data");
	is_synaptic_delay = json_object_get_boolean(rootObject, "is_synaptic_delay");
	is_neuron_delay = json_object_get_boolean(rootObject, "is_neuron_delay");
	delay_method = json_object_get_string(rootObject, "delay_method");

	inference_by_label = json_object_get_boolean(rootObject, "inference_by_label");
	blocked_area = json_object_get_number(rootObject, "blocked_area");

	synapse_noise_sigma = json_object_get_number(rootObject, "synapse_noise_sigma");
	synaptic_delay_mu = json_object_get_number(rootObject, "synaptic_delay_mu");
	synaptic_delay_sigma = json_object_get_number(rootObject, "synaptic_delay_sigma");
	neuron_delay_mu = json_object_get_number(rootObject, "neuron_delay_mu");
	neuron_delay_sigma = json_object_get_number(rootObject, "neuron_delay_sigma");
	neuron_delay_exp_mean = json_object_get_number(rootObject, "neuron_delay_exp_mean");

	std::string learn_method = json_object_get_string(rootObject, "learn_method");
	if (learn_method == "gpgm") {
		num_of_synapse = 2;
		enable_gpgm = true;
	}
	else if (learn_method == "bipolar") {
		num_of_synapse = 1;
		enable_gpgm = false;
	}

	conductance_level = (int)json_object_get_number(rootObject, "conductance_level");
	stdp_window_bipolar = json_object_get_number(rootObject, "stdp_window_bipolar");

	mnist_binary_threshold = json_object_get_number(rootObject, "mnist_binary_threshold");
	spike_rate = json_object_get_number(rootObject, "spike_rate");
	bias_rate_factor = json_object_get_number(rootObject, "bias_rate_factor");
	bias_rate_factor_v = json_object_get_number(rootObject, "bias_rate_factor_v");
	bias_rate_factor_h = json_object_get_number(rootObject, "bias_rate_factor_h");

    json_value_free(rootValue);    // JSON_Value에 할당된 동적 메모리 해제

}
