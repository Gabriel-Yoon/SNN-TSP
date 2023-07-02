# include "rng.h"

//**************************************************************************************************************//

//----------------------------------------------------
rng::rng(const char* param_file) : params(param_file) {
	if (params.enable_random_walk) {
		rng_randomwalk = new py::rng_binomial(0, 0.5);

		if (params.random_walk_step_up == params.random_walk_step_down) {
			if (params.random_walk_step_up != params.random_walk_step || params.random_walk_step_down != params.random_walk_step) {
				//std::clog << "Please check the parameter: (random_walk_step_up, random_walk_step_down) and random_walk_step. Exit." << std::endl;
				assert(false);
			}
		}
	}
	if (params.enable_s2m) {
		rng_s2m = new py::rng_binomial(1, params.s2m_probability);
	}
	if (params.enable_ps2) {
		rng_ps2 = new py::rng_binomial(2, params.ps2_probability);
	}
	if (params.enable_white_noise) {
		rng_white_noise = new py::rng_normal(3, (params.random_walk_step_up - params.random_walk_step_down) / 2, (params.random_walk_step_up + params.random_walk_step_down) / 2);
	}

	rng_randrange = new py::rng_randrange(4, 0, params.num_of_label);
	rng_ureal = new py::rng_ureal(5);
	rng_weight_normal = new py::rng_normal(6, params.wij_init_mu, params.wij_init_sigma);

	if (params.is_neuron_delay) {
		if (params.delay_method == "gauss") {
			rng_neuron_delay_normal = new py::rng_normal(7, params.neuron_delay_mu, params.neuron_delay_sigma);
		}
		else if (params.delay_method == "exponetial") {
			rng_exponential = new py::rng_exponential(8, params.neuron_delay_exp_mean);
		}
	}
	/*
	if (params.is_synaptic_delay) {
		if (params.delay_method == "gauss") {
			synaptic_delay_candidate.reserve(params.num_of_delay_candidate);
			rng_synaptic_delay_normal = new py::rng_normal(9, params.synaptic_delay_mu, params.synaptic_delay_sigma);
			rng_synaptic_delay = new py::rng_randrange(4, 0, params.num_of_delay_candidate);
		}
		for (int i = 0; i < params.num_of_delay_candidate; i++) {
			synaptic_delay_value = rng_synaptic_delay_normal->get_val();
			if (synaptic_delay_value < 0.0) { synaptic_delay_value = 0.0; }
			else if (synaptic_delay_value > params.refractory_time) { synaptic_delay_value = params.refractory_time; }
			synaptic_delay_candidate.push_back(synaptic_delay_value);
		}
	}
	*/

	if (params.enable_sns) {
		rng_noise_normal = new py::rng_normal(10, 1.0, params.synapse_noise_sigma);
	}

}
//**************************************************************************************************************//