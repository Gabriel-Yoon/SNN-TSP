# include "rng.h"

//**************************************************************************************************************//

//----------------------------------------------------
rng::rng(param& _params){
	if (_params.enable_random_walk) {
		rng_randomwalk = new py::rng_binomial(0, 0.5);

		if (_params.random_walk_step_up == _params.random_walk_step_down) {
			if (_params.random_walk_step_up != _params.random_walk_step || _params.random_walk_step_down != _params.random_walk_step) {
				//std::clog << "Please check the parameter: (random_walk_step_up, random_walk_step_down) and random_walk_step. Exit." << std::endl;
				assert(false);
			}
		}
	}
	if (_params.enable_s2m) {
		rng_s2m = new py::rng_binomial(1, _params.s2m_probability);
	}
	if (_params.enable_ps2) {
		rng_ps2 = new py::rng_binomial(2, _params.ps2_probability);
	}
	if (_params.enable_white_noise) {
		rng_white_noise = new py::rng_normal(3, (_params.random_walk_step_up - _params.random_walk_step_down) / 2, (_params.random_walk_step_up + _params.random_walk_step_down) / 2);
	}

	rng_randrange = new py::rng_randrange(4, 0, _params.num_of_label);
	rng_ureal = new py::rng_ureal(5);
	rng_weight_normal = new py::rng_normal(6, _params.wij_init_mu, _params.wij_init_sigma);

	if (_params.is_neuron_delay) {
		if (_params.delay_method == "gauss") {
			rng_neuron_delay_normal = new py::rng_normal(7, _params.neuron_delay_mu, _params.neuron_delay_sigma);
		}
		else if (_params.delay_method == "exponetial") {
			rng_exponential = new py::rng_exponential(8, _params.neuron_delay_exp_mean);
		}
	}
	/*
	if (_params.is_synaptic_delay) {
		if (_params.delay_method == "gauss") {
			synaptic_delay_candidate.reserve(_params.num_of_delay_candidate);
			rng_synaptic_delay_normal = new py::rng_normal(9, _params.synaptic_delay_mu, _params.synaptic_delay_sigma);
			rng_synaptic_delay = new py::rng_randrange(4, 0, _params.num_of_delay_candidate);
		}
		for (int i = 0; i < _params.num_of_delay_candidate; i++) {
			synaptic_delay_value = rng_synaptic_delay_normal->get_val();
			if (synaptic_delay_value < 0.0) { synaptic_delay_value = 0.0; }
			else if (synaptic_delay_value > _params.refractory_time) { synaptic_delay_value = _params.refractory_time; }
			synaptic_delay_candidate.push_back(synaptic_delay_value);
		}
	}
	*/

	if (_params.enable_sns) {
		rng_noise_normal = new py::rng_normal(10, 1.0, _params.synapse_noise_sigma);
	}

}
//**************************************************************************************************************//