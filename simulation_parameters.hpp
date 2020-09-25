#ifndef SIMULATION_PARAMETERS
#define SIMULATION_PARAMETERS

#include <string>
#include <vector>

/* Simulation parameter definitions */ 

#define FLOAT_EPSILON_TIME 1e-10

using namespace std;

struct simulation_parameters {

    // Timestep for RandomWalk
    double timestep;
    double timestep_rng;
    double timestep_injection;

    // TSP Inhibition Weight Setup
    double same_WTA_diff_cities;
    double adj_WTA_same_cities;
    double non_adj_WTA_same_cities;
    double b_value;
    double gradient;

    // Spike modulation
    double wlr_width;
    double stdp_window;
    double refractory_time;
    double delay_scanchain;
    double delay_spikein2out;
    double delay_spikeout2wlr;
    double delay_spikeout2wlr_data;
    double delay_spikeout2wlr_model;
    double delay_spikeout2wup;
    double delay_spikeout2wup_data;
    double delay_spikeout2wup_model;
    double delay_spikeout2td3;
    double tset_width;
    double treset_width;

    // Leak
    double pt_init;
    double pt_alpha;
    double pt_lk_tau;

    // Synapse
    double wt_delta_g_set;
    double wt_delta_g_set_sigma_cycle;
    int wt_delta_g_set_seed_cycle;
    double wt_delta_g_set_sigma_device;
    int wt_delta_g_set_seed_device;
    double wt_delta_g_reset;
    double wt_delta_g_reset_sigma_cycle;
    int wt_delta_g_reset_seed_cycle;
    double wt_delta_g_reset_sigma_device;
    int wt_delta_g_reset_seed_device;
    double wt_reset_rate;
    int wt_reset_rate_seed;

    double max_weight;
    double max_weight_sigma;
    int max_weight_seed;
    double min_weight;
    double min_weight_sigma;
    int min_weight_seed;

    double random_walk_step;
    double random_walk_mismatch;

    int rng_for_random_walk;
    int rng_seed_for_random_walk;

    bool enable_random_walk;
    bool enable_gpgm;
    bool enable_simulated_annealing;
    bool enable_firing_probability;

    bool hw_ISO_MOD;
    bool hw_CAP_ISO;
    bool hw_RES_EN;
    bool hw_RES_BLK;

    simulation_parameters();
    void print_parameters();

};

enum : int {
    spk_type_both,
    spk_type_ext,
    spk_type_int,
};

enum : int {
    rng_lfsr32,
    rng_lfsr16,
    rng_lfsr8,
    rng_mt
};

enum : int {
	side_v,
	side_h
};

extern char side_chr[2];

#endif