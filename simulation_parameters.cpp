#include <iostream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
//#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>   // for lexical_cast<>()

#include "parameters.hpp"

using namespace std;

simulation_parameters::simulation_parameters(string param_file) {

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(param_file, tree);

    // Load parameters for core0
    // Usage: tree.get<TYPE>(cpu_str + "PARAM_NAME", DEFAULT_VALUE);
    string cpu_str = "core0.";

    timestep = tree.get<double>("timestep", 1e-6);
    timestep_rng = tree.get<double>("timestep_rng", 10e-6);

    wlr_width = tree.get<double>("wlr_width", 40e-9);
    stdp_window = tree.get<double>("stdp_window", 2e-3) - FLOAT_EPSILON_TIME;
    refractory_time = tree.get<double>("refractory_time", 4e-3) - FLOAT_EPSILON_TIME;
    delay_spikein2out = tree.get<double>("delay_spikein2out", 100e-9);
    delay_spikeout2wlr = tree.get<double>("delay_spikeout2wlr", 0.0);
    delay_spikeout2wlr_data = tree.get<double>("delay_spikeout2wlr_data", 0.0);
    delay_spikeout2wlr_model = tree.get<double>("delay_spikeout2wlr_model", 0.0);
    delay_spikeout2wup = tree.get<double>("delay_spikeout2wup", 0.0);
    delay_spikeout2wup_data = tree.get<double>("delay_spikeout2wup_data", 2e-3);
    delay_spikeout2wup_model = tree.get<double>("delay_spikeout2wup_model", 2e-3);
    delay_spikeout2td3 = tree.get<double>("delay_spikeout2td3", 2e-3);
    tset_width = tree.get<double>("test_width", 2e-3);
    treset_width = tree.get<double>("treset_width", 2e-3);

    pt_init = tree.get<double>("pt_init", 0.0);
    pt_alpha = tree.get<double>("pt_alpha", 0.1);
    pt_lk_tau = tree.get<double>("pt_lk_tau", 1e-3);

    // for circuit
    wt_delta_g_set = tree.get<double>("wt_delta_g_set", 0.01);
    wt_delta_g_set_sigma_cycle = tree.get<double>("wt_delta_g_set_sigma_cycle", 0.0);
    wt_delta_g_set_seed_cycle = tree.get<double>("wt_delta_g_set_seed_cycle", 3);
    wt_delta_g_set_sigma_device = tree.get<double>("wt_delta_g_set_sigma_device", 0.0);
    wt_delta_g_set_seed_device = tree.get<double>("wt_delta_g_set_seed_device", 4);
    wt_delta_g_reset = tree.get<double>("wt_delta_g_reset", -0.01);
    wt_delta_g_reset_sigma_cycle = tree.get<double>("wt_delta_g_reset_sigma_cycle", 0.0);
    wt_delta_g_reset_seed_cycle = tree.get<double>("wt_delta_g_reset_seed_cycle", 5);
    wt_delta_g_reset_sigma_device = tree.get<double>("wt_delta_g_reset_sigma_device", 0.0);
    wt_delta_g_reset_seed_device = tree.get<double>("wt_delta_g_reset_seed_device", 6);
    wt_reset_rate = tree.get<double>("wt_reset_rate", 1.0);
    wt_reset_rate_seed = tree.get<int>("wt_reset_rate_seed", 7);
    max_weight = tree.get<double>("max_weight", 10.0);
    max_weight_sigma = tree.get<double>("max_weight_sigma", 0.0);
    max_weight_seed = tree.get<int>("max_weight_seed", 8);
    min_weight = tree.get<double>("min_weight", 0.0);
    min_weight_sigma = tree.get<double>("min_weight_sigma", 0.0);
    min_weight_seed = tree.get<int>("min_weight_seed", 9);

    enable_learning = tree.get<bool>("enable_learning", true);
    enable_ps2 = tree.get<bool>("enable_ps2", false);
    string stochastic_method = tree.get<string>("stochastic_method", "random_walk");
    if(stochastic_method == "random_walk") {
        enable_random_walk = true;
    } else if(stochastic_method == "stochastic_vth") {
        enable_stochastic_vth = true;
    } else {
        cerr <<"Invalid value specified for stochastic_method: " << stochastic_method << ". Exiting." << endl;
        exit(0);
    }
    enable_gpgm = tree.get<bool>("enable_gpgm", true);
    
    inference_by_label = tree.get<bool>("inference_by_label", false);

    hw_ISO_MOD = tree.get<bool>("hw_ISO_MOD", false);
    hw_RES_EN = tree.get<bool>("hw_RES_EN", false);
    hw_RES_BLK = tree.get<bool>("hw_RES_BLK", false);

    random_walk_step = tree.get<double>("random_walk_step", 0.06);
    random_walk_mismatch = tree.get<double>("random_walk_mismatch", 0.0);

    string rng_type = tree.get<string>("rng_for_random_walk", "lfsr32");
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
    rng_seed_for_random_walk = tree.get<int>("rng_seed_for_random_walk", 2);

    rng_type = tree.get<string>("rng_for_ps2", "lfsr32");
    if(rng_type == "lfsr32") {
        rng_for_ps2 = rng_lfsr32;
    } else if(rng_type == "lfsr16") {
        rng_for_ps2 = rng_lfsr16;
    } else if(rng_type == "lfsr8") {
        rng_for_ps2 = rng_lfsr8;
    } else if(rng_type == "mersenne") {
        rng_for_ps2 = rng_mt;
    } else {
        cerr << "Invalid value specified for rng_for_ps2: " << rng_type << ". Exiting." << endl;
        exit(0);
    }
    rng_seed_for_ps2 = tree.get<int>("rng_seed_for_ps2", 2);    
}