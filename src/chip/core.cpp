#include <math.h>
#include <algorithm>
#include <unistd.h>

#include "core.h"
#include "spk.h"

enum {
    wij_gp,
    wij_gm
};

spk *spk_null;

core::core(const char* param_file) : params(param_file)
{
    // NEED MODIFICATION for num_neurons
    // link num_neurons with TSP num_city
	num_neurons[side_v] = 100;
    num_neurons[side_h] = 100;
    std::string cpu_str = "core0.";
    export_ptn_file[0] = cpu_str + "export_ptn_file_v"+ "ptn_v.dat";
    export_ptn_file[1] = cpu_str + "export_ptn_file_h"+ "ptn_h.dat";
}

void core::initialize(char* fextspk, char* fexttime, char* fwij, char* fwij_gp, char* fwij_gm){

    synapses.resize(num_neurons[side_v]);
    layers.resize(num_neurons[side_v]);
    for(int i = 0; i < num_neurons[side_v]; i++) {
        synapses[i].resize(num_neurons[side_h]);
        layers.resize(num_neurons[side_h]);
    }
        int i, j;
        for(i = 0; i < num_neurons[side_v]; i++) {
            neuron Neuron = layers[side_v][i];
            Neuron._memV = 0.0;
            Neuron._Vth = 1;
            Neuron._lastSpkTime = -1;
            Neuron._lastSpkTimeIN_PAUSE = -1;
            Neuron._lastSpkTimeST_PAUSE = -1;
        }
        for(i = 0; i < num_neurons[side_h]; i++) {
            neuron Neuron = layers[side_h][i];
            Neuron._memV = 0.0;
            Neuron._Vth = 1;
            Neuron._lastSpkTime = -1;
            Neuron._lastSpkTimeIN_PAUSE = -1;
            Neuron._lastSpkTimeST_PAUSE = -1;
        }

        // NEED MODIFICATION
        // Load python generated external spikes
        if((fextspk != NULL) && (fexttime != NULL)) {
                ext_spike_load(fextspk, fexttime);
        } else {
            cout<< "Error: external spike files are missing. Exit." <<endl;
            exit(0);
        }

        // NEED MODIFICATION
        // Load synapse Gp, Gm Weights first!!

        for(i = 0; i < num_neurons[side_v]; i++) {
            for(j = 0; j < num_neurons[side_h]; j++) {
                synapse Synapse = synapses[i][j];
                if(Synapse.Gp > params.max_weight) {
                    Synapse.Gp = params.max_weight;
                } else if(Synapse.Gp < params.min_weight) {
                    Synapse.Gp = params.min_weight;
                }
            }
        }

        for(i = 0; i < num_neurons[side_v]; i++) {
            for(j = 0; j < num_neurons[side_h]; j++) {
                synapse Synapse = synapses[i][j];
                if(Synapse.Gm > params.max_weight) {
                    Synapse.Gm = params.max_weight;
                } else if(Synapse.Gm < params.min_weight) {
                    Synapse.Gm = params.min_weight;
                }
            }
        }

        spk_null = new spk;
        spk_null->_spkTime = INFINITY;
}

void core::print_params() {
	// Should add neuron info, synapse info, etc.
	
	std::cout << "Simulation parameter list" << std::endl;
	std::cout << "--------------------------neuron------------------------ " << std::endl;
	std::cout << "neurons_v_data	: " << params.neurons_visible_data << std::endl;
	std::cout << "neurons_v_label	: " << params.neurons_visible_label << std::endl;
	std::cout << "neurons_h_data	: " << params.neurons_hidden_data << std::endl;
	std::cout << "neurons_v_bias	: " << params.neurons_visible_bias << std::endl;
	std::cout << "neurons_h_bias	: " << params.neurons_hidden_bias << std::endl;
	std::cout << "--------------------------time-------------------------- " << std::endl;
	std::cout << "timestep 			:" << params.timestep << std::endl;
	std::cout << "timestep_rng 		:" << params.timestep_rng << std::endl;
	std::cout << "refractory time	: " << params.refractory_time << std::endl;
	std::cout << "steps_transition	: " << params.steps_transition << std::endl;
	std::cout << "steps_data		: " << params.steps_data << std::endl;
	std::cout << "steps_model		: " << params.steps_model << std::endl;
	std::cout << "----------------------pulse delay----------------------- " << std::endl;
	std::cout << "delay_spikein2out			: " << params.delay_spikein2out << std::endl;
	std::cout << "wlr_width					: " << params.wlr_width << std::endl;
	std::cout << "delay_spikeout2wup		: " << params.delay_spikeout2wup << std::endl;
	std::cout << "delay_spikeout2wup_data	: " << params.delay_spikeout2wup_data << std::endl;
	std::cout << "delay_spikeout2wup_model	: " << params.delay_spikeout2wup_model << std::endl;
	std::cout << "delay_spikeout2td3		: " << params.delay_spikeout2td3 << std::endl;
	std::cout << "tset_width				: " << params.tset_width << std::endl;
	std::cout << "treset_width				: " << params.treset_width << std::endl;
	std::cout << "--------------------------mode-------------------------- " << std::endl;
	std::cout << "enable_random_walk 	:" << params.enable_random_walk << std::endl;
	std::cout << "enable_white_noise 	:" << params.enable_white_noise << std::endl;
	std::cout << "enable_sigmoid 		:" << params.enable_sigmoid << std::endl;
	std::cout << "enable_ps2 			:" << params.enable_ps2 << std::endl;
	std::cout << "enable_s2m 			:" << params.enable_s2m << std::endl;
	std::cout << "enable_sns 			:" << params.enable_sns << std::endl;
	std::cout << "is_real_data 			:" << params.is_real_data << std::endl;
	std::cout << "is_synaptic_delay 	:" << params.is_synaptic_delay << std::endl;
	std::cout << "is_neuron_delay 		:" << params.is_neuron_delay << std::endl;
	std::cout << "inference_by_label 	:" << params.inference_by_label << std::endl;
	std::cout << "--------------------------other------------------------- " << std::endl;
	std::cout << "data, label spike_rate 	:" << params.spike_rate << std::endl;
	std::cout << "bias_rate_factor 			:" << params.bias_rate_factor << std::endl;
	std::cout << "bias spike 				:" << params.spike_rate * params.bias_rate_factor << std::endl;
	std::cout << "wt_delta_g_set 			:" << std::fixed << params.wt_delta_g_set << std::endl;
	std::cout << "wt_delta_g_reset 			:" << std::fixed << params.wt_delta_g_reset << std::endl;
	std::cout << "pt_threshold 				:" << params.pt_threshold << std::endl;
	
	// Should add ON/OFF mode to print below
	/*
	if (params.inference_by_label) {
		std::cout << "blocked_area : " << params.blocked_area << std::endl;
	}
	if (params.enable_sigmoid) {
		std::cout << "sigmoid_mid : " << params.sigmoid_mid << std::endl;
		std::cout << "sigmoid_temp : " << params.sigmoid_temp << std::endl;
	}
	if (params.enable_ps2) {
		std::cout << "ps2_probability : " << params.ps2_probability << std::endl;
	}
	if (params.enable_s2m) {
		std::cout << "s2m_probability : " << params.s2m_probability << std::endl;
	}
	if (params.is_real_data) {
		std::cout << "conductance_level : " << params.conductance_level << std::endl;
	}
	if (params.enable_sns) {
		std::cout << "synapse_noise_sigma : " << params.synapse_noise_sigma << std::endl;
	}
	if (params.is_synaptic_delay) {
		std::cout << "synaptic_delay_mu : " << params.synaptic_delay_mu << std::endl;
		std::cout << "synaptic_delay_sigma : " << params.synaptic_delay_sigma << std::endl;
	}
	if (params.is_neuron_delay) {
		if (params.delay_method == "gauss") {
			std::cout << "neuron_delay_mu : " << params.neuron_delay_mu << std::endl;
			std::cout << "neuron_delay_sigma : " << params.neuron_delay_sigma << std::endl;
		}
		else if (params.delay_method == "exponential") {
			std::cout << "neuron_delay_exp_mean : " << params.neuron_delay_exp_mean << std::endl;
		}
		else {
			assert(false);
		}
	}
	*/
}

void core::weight_load(int cell_type, std::string fweight) {
	std::ifstream is;
	is.open(fweight, std::ios::binary);
	if (is.fail()) {
		std::cout << "Error opening file " << fweight << ". Exit." << std::endl;
		exit(1);
	}

	//Check file size
	is.seekg(0, std::ios::end);
	auto eofpos = is.tellg();
	is.clear();

	is.seekg(0, std::ios::beg);
	auto begpos = is.tellg();
	long int fsize = eofpos - begpos;
	long int dsize_wt = sizeof(double) * num_neurons[side_v] * num_neurons[side_h];
	if ((fsize != dsize_wt)) {
		std::clog << "Unexpected file size " << fsize << ". Exit." << std::endl;
		exit(1);
	}

	double weight = 0.0;

	for (int i = 0; i < num_neurons[side_v]; i++) {
		for (int j = 0; j < num_neurons[side_h]; j++) {
			is.read((char*)&weight, sizeof(double));
			if (cell_type == wij_gp) {
				if (weight > params.max_weight) {
					weight = params.max_weight;
				}
				else if (weight < params.min_weight) {
					weight = params.min_weight;
				}
				synapses[i][j].Gp = weight;
			}
			else {
				if (weight > params.max_weight) {
					weight = params.max_weight;
				}
				else if (weight < params.min_weight) {
					weight = params.min_weight;
				}
				synapses[i][j].Gm = weight;
			}
		}
	}
	is.close();
}

void core::weight_save(int cell_type, std::string filename) {
	std::ofstream os;
	os.open(filename, std::ios::binary);

	for (int i = 0; i < num_neurons[side_v]; i++) {
		for (int j = 0; j < num_neurons[side_h]; j++) {
			if (cell_type == wij_gp) {
				os.write((char*)&(synapses[i][j].Gp), sizeof(double));
			}
			else {
				os.write((char*)&(synapses[i][j].Gm), sizeof(double));
			}
		}
	}
	os.close();
}

void core::ext_spike_load(double tend) {

    cout << "[START] EXTERNAL SPIKE LOAD" << endl;
    double timestep_injection = 10e-5;
    int neuron = 0; // start city : if city 1 is the start, neuron =0;
    double time;
    double injection_step = tend / timestep_injection;
    
    priority_queue<pair<double, spk_one*>, vector<pair<double, spk_one*>>, greater<pair<double, spk_one*>>> queue_ext_pri;

    for (int i = 0; i < injection_step; i++) {
        spk_one* spk = new spk_one;
        spk->time = i * timestep_injection;
        spk->side = side_v;
        spk->neuron = neuron;
        queue_ext_pri.push(make_pair(spk->time, spk));
    }

    
    while (!queue_ext_pri.empty()) {
        spk_one* spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        // Formulate Spike Template
        spk* spk_ext = new spk;
        spk_ext->_spk.push_back(make_pair(spk_one->side, spk_one->neuron));

        double time_fire2 = spk_one->time + params.delay_spikein2out;

        // 1. Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + params.refractory_time + FLOAT_EPSILON_TIME;
        spk* spk_ref = new spk(*spk_ext);
        spk_ref->_spkTime = ref_end_time;
        if (params.hw_CAP_ISO) {
            spk_ref->_iso = -1;
        }
        queue_ext.push(make_pair(ref_end_time, spk_ref));

        spk_ext->_iso = 0;
        // 2. Create last_spk_st update event for ST_PAUSE
        spk* spk_st_update = new spk(*spk_ext);
        spk_st_update->_spkTime = spk_one->time;
        spk_st_update->_st = true;
        queue_ext.push(make_pair(spk_st_update->_spkTime, spk_st_update));

        // 3. Create reset event
        if (params.hw_RES_EN) {
            spk* spk_ext_reset = new spk(*spk_ext);
            spk_ext_reset->_spkTime = time_fire2;
            spk_ext_reset->_reset = true;
            queue_ext.push(make_pair(time_fire2, spk_ext_reset));
        }

        // Push spike event to queue
        spk_ext->_spkTime = time_fire2 + params.delay_spikeout2wlr + params.wlr_width;
        if (params.hw_CAP_ISO) {
            spk_ext->_iso = 1;
        }
        queue_ext.push(make_pair(spk_ext->_spkTime, spk_ext));

        delete spk_one;
    }
    cout << "[_END_] EXTERNAL SPIKE LOAD\n" << endl;

}

int core::get_spk(spk **spk_now, int *which_spk) {
    int num_spk_ext = queue_ext.size();
    int num_spk_int = queue_spk.size();

    if(num_spk_ext == 0) {
        if(num_spk_int == 0) { // No More event
            *spk_now = spk_null;
            return -1;
        }
        *spk_now = queue_spk.top().second;
        *which_spk = spk_type_int;
        return 0;
    } else if(num_spk_int == 0) {
        *spk_now = queue_ext.top().second;
        *which_spk = spk_type_ext;
        return 0;
    }

    spk *spk_ext = queue_ext.top().second;
    spk *spk_int = queue_spk.top().second;
    if(fabs(spk_ext->_spkTime - spk_int->_spkTime) < FLOAT_EPSILON_TIME) {
        if((spk_ext->_reset == spk_int->_reset) && (spk_ext->_st == spk_int->_st)) {
            spk_ext->merge(*spk_int);
            *spk_now = spk_ext;
            *which_spk = spk_type_both;
        } else {
            *spk_now = spk_ext;
            *which_spk = spk_type_ext;
        }
    } else if(spk_ext->_spkTime < spk_int->_spkTime) {
        *spk_now = spk_ext;
        *which_spk = spk_type_ext;
    } else { // spk_ext->time > spk_int->time
        *spk_now = spk_int;
        *which_spk = spk_type_int;
    }

    return 0;
}

int core::compare_threshold(double tnow) {
    spk *new_spk = new spk;
    spk *new_spk_reset = new spk;
    double time_in_ref = tnow - params.refractory_time;

    for(int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++) { // exclude bias neuron
        // if(params.enable_ps2 && rng_ps2->get_val()) continue;
        neuron Neuron = layers[side_h][h_idx];
        bool compared = Neuron._memV > Neuron._Vth;
        bool not_in_ref = Neuron._lastSpkTime < time_in_ref;

        if(!compared) continue;

        /*
        // CAP_ISO = ST_PAUSE || (IN_PAUSE && hw_ISO_MOD)
        // ST_PAUSE == IN_PAUSE in hidden neruons (always internal)
        // ST_PAUSE = high while in refractory time by INT_SPK
        // RESET = (CAP_ISO_RE && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = (INT_SPK && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = INT_SPK && (RES_EN || !CAP_ISO || !hw_RES_BLK)
        // INT_SPK = compared
        // CAP_ISO = !not_in_ref
        if(params.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
        }
        */
        new_spk_reset->_spk.push_back(make_pair(side_h, h_idx));

        if(not_in_ref) {

            // ST_PAUSE is always selected for hidden neurons
            // No need to update lask_spk_in
            Neuron._lastSpkTime = tnow;

            new_spk->_spk.push_back(make_pair(side_h, h_idx));
        }
    }

    for(int v_idx = 0; v_idx < num_neurons[side_v]; v_idx++) {
        //if(param.enable_ps2 && rng_ps2->get_val()) continue;
        neuron Neuron = layers[side_v][v_idx];
        bool compared = Neuron._memV > Neuron._Vth;
        bool not_in_ref = Neuron._lastSpkTime < time_in_ref;
        /*
        bool not_in_ref_in = (last_spk_in[side_v][v_idx] < time_in_ref);
                
        if(not_in_ref_in) {
            last_spk_in[side_v][v_idx] = tnow;
        }
        */

        if(!compared) continue;

        /*
        // Neurons in external spike mode
        if((v_idx < v_idx_start) || (v_idx >= v_idx_end)) {
            //Potential reset by external spikes is handled in potential_update_by_spk()
            bool cap_iso = !not_in_ref || (!not_in_ref_in && param.hw_ISO_MOD);
            bool res_en_int = param.hw_RES_EN && not_in_ref_in && param.hw_ISO_MOD;
            bool res_blk = param.hw_RES_BLK && cap_iso;

            if(res_en_int || !res_blk) {
                new_spk_reset->spk.push_back(make_pair(side_v, v_idx));
            }
            continue;
        }

        //Neurons in internal spike mode
        // RESET = (CAP_ISO_RE && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = (INT_SPK && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = INT_SPK && (RES_EN || !CAP_ISO || !hw_RES_BLK)
        if(param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_v, v_idx));
        }
        */

        new_spk_reset->_spk.push_back(make_pair(side_v, v_idx));
        if(not_in_ref) {

            Neuron._lastSpkTime = tnow;

            new_spk->_spk.push_back(make_pair(side_v, v_idx));
        }
    }

    double time_fire2 = tnow + params.delay_spikein2out;

    // RESET EVENT
    int spike_flag = 0;
    if(new_spk_reset->_spk.size() != 0) {
        new_spk_reset->_spkTime = time_fire2;
        new_spk_reset->_reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        spike_flag = 1;
    } else {
        delete new_spk_reset;
    }

    if(new_spk->_spk.size() != 0) {
        // SPIKE EVENT
        new_spk->_spkTime = time_fire2 + params.delay_spikeout2wlr + params.wlr_width;
        queue_spk.push(make_pair(new_spk->_spkTime, new_spk));
        spike_flag = 1;
        // DUMMY EVENT to kick compare_threshold at the end of refractory time
        spk *spk_ref = new spk;
        spk_ref->_spkTime = tnow + params.refractory_time + FLOAT_EPSILON_TIME;
        queue_spk.push(make_pair(spk_ref->_spkTime, spk_ref));

		/*
        // WUP event
        if(param.enable_learning) {
            if(!param.enable_gpgm) {
                spk *new_spk_wup = new spk(*new_spk); // WUP event
                new_spk_wup->time = time_fire2 + param.delay_spikeout2wup;
                queue_wup_spk.push(make_pair(new_spk_wup->time, new_spk_wup));
            } else {
                spk *new_spk_wup_v = new spk;
                spk *new_spk_wup_h = new spk;
                for(auto it = new_spk->spk.begin(); it != new_spk->spk.end(); it++) {
                    if(it->first == side_v) {
                        new_spk_wup_v->spk.push_back(*it);
                    } else {
                        new_spk_wup_h->spk.push_back(*it);
                    }
                }
                if(new_spk_wup_v->spk.size() > 0) {
                    if(phase_now == sm_data_phase) {
                        new_spk_wup_v->time = time_fire2 + param.delay_spikeout2td3;
                    } else {
                        new_spk_wup_v->time = time_fire2 + param.delay_spikeout2td3 - param.tset_width;
                    }
                    queue_wup_spk.push(make_pair(new_spk_wup_v->time, new_spk_wup_v));
                } else {
                    delete new_spk_wup_v;
                }
                if(new_spk_wup_h->spk.size() > 0) {
                    if(phase_now == sm_data_phase) {
                        new_spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_data;
                    } else {
                        new_spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_model;
                    }
                    queue_wup_spk.push(make_pair(new_spk_wup_h->time, new_spk_wup_h));
                } else {
                    delete new_spk_wup_h;
                }
            }
			
        } */
    } else {
        delete new_spk;
    }

    return spike_flag;
};

template<int is_spk, int is_rng> void core::run_loop(double tnow, double tpre, spk &spk_now, int which_spk, double &simtick, int &new_spk) {
    /*EVENT_TIME_DUMP(tnow);
    if(is_spk) {
        if(params.enable_learning)
            weight_update(tnow);
    }
    */

    potential_update_by_leak(tnow-tpre);
    export_ptn(tnow);

    if(is_spk) {
        potential_update_by_spk(spk_now, which_spk);
        export_ptn(tnow);
        spk *spk_export;
        if(which_spk == spk_type_ext) {
            spk_export = queue_ext.top().second;
            export_spk(*spk_export, spk_type_ext);
        } else if(which_spk == spk_type_int) {
            spk_export = queue_spk.top().second;
            export_spk(*spk_export, spk_type_int);
        } else if(which_spk == spk_type_both) {
            spk_export = queue_ext.top().second;
            export_spk(*spk_export, spk_type_ext);
            spk_export = queue_spk.top().second;
            export_spk(*spk_export, spk_type_int);
        }
    }

    if(is_rng) {
        if(params.enable_random_walk) {
            potential_update_by_random_walk();
            export_ptn(tnow);
        } else if(params.enable_stochastic_vth) {
            threshold_update_stochastic();
        }
        simtick += params.timestep_rng;
    }

    new_spk = compare_threshold(tnow);
}

double core::run() {

    double tend = 1.0;
    double tnow = 0.0;
    double tpre = 0.0;
    double simtick = params.timestep_rng;

    spk *spk_now;
    int which_spk = 0;
    int is_spk = 0;
    int new_spk = 0;

    long int loop_count = 0;
    /*
    if(!params.enable_gpgm) {
        weight_save(wij_gp, "wij_first.bin");
    } else {
        weight_save(wij_gp, "wij_first_gp.bin")
        weight_save(wij_gm, "wij_first_gm.bin")
    }
    */
    get_spk(&spk_now, &which_spk);

    cout << setprecision(9);

    while(1) {

#ifdef DEBUG_LOOP
                cout << "------LOOP------" << endl;
#endif
                // get spike event
                if(is_spk) {
                    if(which_spk == spk_type_ext) {
                        delete queue_ext.top().second;
                        queue_ext.pop();
                    } else if(which_spk == spk_type_int) {
                        delete queue_spk.top().second;
                        queue_spk.pop();
                    } else if(which_spk == spk_type_both) {
                        delete queue_ext.top().second;
                        queue_ext.pop();
                        delete queue_spk.top().second;
                        queue_spk.pop();
                    }
                }

                if(new_spk || is_spk) {
                    new_spk = 0;
                    is_spk = 0;
                    get_spk(&spk_now, &which_spk);
                }

#ifdef DEBUG_LOOP
                cout << "simtick " << simtick << " spk time " << spk_now->time << endl;
#endif
                if(fabs(simtick - spk_now->_spkTime) < FLOAT_EPSILON_TIME) { // simtick == spk_now.time
                    tnow = simtick;
                    if(tnow > tend) break;
                    is_spk = 1;
                    if(spk_now->_reset == true) {
                        // potential_reset(*spk_now); // Reset before or after run_loop()???
                        for(auto it = spk_now->_spk.begin(); it != spk_now->_spk.end(); it++) {
                            layers[it->first][it->second].memV_Reset();
		                }
                        run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                        tpre = tnow;
                    } else if(spk_now->_st == true) {
                        for(auto it = spk_now->_spk.begin(); it != spk_now->_spk.end(); it++) {
                            layers[it->first][it->second].updateLastSpkTime(spk_now->_spkTime);
		                }
                        run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                        tpre = tnow;
                    } else {
                        run_loop<1, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                        tpre = tnow;
                    }
                } else if(simtick < spk_now->_spkTime) {
                    tnow = simtick;
                    if(tnow > tend) break;
                    is_spk = 0;
                    run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                    tpre = tnow;
                } else { // if simtick > spk_now.time
                    tnow = spk_now->_spkTime;
                    if(tnow > tend) break;
                    is_spk = 1;
                    if(spk_now->_reset == true) {
                        for(auto it = spk_now->_spk.begin(); it != spk_now->_spk.end(); it++) {
                            layers[it->first][it->second].memV_Reset();
		                }
                    } else if(spk_now->_st == true) {
                        for(auto it = spk_now->_spk.begin(); it != spk_now->_spk.end(); it++) {
                            layers[it->first][it->second].updateLastSpkTime(spk_now->_spkTime);
		                }
                    } else {
                        run_loop<1, 0>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                        tpre = tnow;
                    }
                }

                loop_count++;
    }
    /*
    cout << "Save final wij matrix to wij_last.bin" << endl;
    if(!params.enable_gpgm) {
        weight_save(wij_gp, "wij_last.bin");
    } else {
        weight_save(wij_gp, "wij_last_gp.bin");
        weight_save(wij_gm, "wij_last_gm.bin");
    }
    cout << "loop_count " << loop_count << endl;
    
    close_export();
    */
    return tnow;

}