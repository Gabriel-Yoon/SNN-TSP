#include <iostream>
#include <sstream>

#include "simulation_parameters.hpp"

using namespace std;

void simulation_parameters::print_parameters(){
    cout << this->pt_alpha << endl;
}

simulation_parameters::simulation_parameters() {

    timestep = 1e-6;
    timestep_rng = 10e-6;

    wlr_width = 40e-9;
    stdp_window =2e-3 - FLOAT_EPSILON_TIME;
    refractory_time = 4e-3 - FLOAT_EPSILON_TIME;
    delay_spikein2out = 100e-9;
    delay_spikeout2wlr = 0.0;
    delay_spikeout2wlr_data = 0.0;
    delay_spikeout2wlr_model = 0.0;
    delay_spikeout2wup = 0.0;
    delay_spikeout2wup_data = 2e-3;
    delay_spikeout2wup_model = 2e-3;
    delay_spikeout2td3 = 2e-3;
    tset_width = 2e-3;
    treset_width = 2e-3;

    pt_init = 0.0;
    pt_alpha = 0.1;
    pt_lk_tau = 1e-3;

    // for circuit
    wt_delta_g_set = 0.01;
    wt_delta_g_set_sigma_cycle = 0.0;
    wt_delta_g_set_seed_cycle = 3;
    wt_delta_g_set_sigma_device = 0.0;
    wt_delta_g_set_seed_device = 4;
    wt_delta_g_reset = -0.01;
    wt_delta_g_reset_sigma_cycle = 0.0;
    wt_delta_g_reset_seed_cycle = 5;
    wt_delta_g_reset_sigma_device = 0.0;
    wt_delta_g_reset_seed_device = 6;
    wt_reset_rate = 1.0;
    wt_reset_rate_seed = 7;
    max_weight = 10.0;
    max_weight_sigma = 0.0;
    max_weight_seed = 8;
    min_weight = 0.0;
    min_weight_sigma = 0.0;
    min_weight_seed = 9;

    enable_learning = true;
    string stochastic_method = "random_walk";
    if(stochastic_method == "random_walk") {
        enable_random_walk = true;
    } else if(stochastic_method == "stochastic_vth") {
        enable_stochastic_vth = true;
    } else {
        cerr <<"Invalid value specified for stochastic_method: " << stochastic_method << ". Exiting." << endl;
        exit(0);
    }
    enable_gpgm = true;

    hw_ISO_MOD = false;
    hw_RES_EN = false;
    hw_RES_BLK = false;

    random_walk_step = 0.06;
    random_walk_mismatch = 0.0;

    string rng_type = "lfsr32";
    if(rng_type == "lfsr32") {
        rng_for_random_walk = rng_lfsr32;
    } else if(rng_type == "lfsr16") {
        rng_for_random_walk = rng_lfsr16;
    } else if(rng_type == "lfsr8") {
        rng_for_random_walk = rng_lfsr8;
    } else if(rng_type == "mersenne") {
        rng_for_random_walk = rng_mt;
    } else {
        cerr << "Invalid value specified for rng_for_random_walk: " << rng_type << ". Exiting." << endl;
        exit(0);
    }

    rng_seed_for_random_walk = 2;
}  