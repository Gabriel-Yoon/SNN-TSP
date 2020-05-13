#ifndef CORE
#define CORE

#include <vector>
#include <string>

#ifdef ENABLE_AVX
#include <x86intrin.h>
#endif
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
        
        // Simulation Parameter Declare
        simulation_parameters param;

        // Neuron
        int num_city = 5;

        int neurons_visible_city;
        int neurons_visible_bias;
        int neurons_hidden_city;
        int neurons_hidden_bias;
        int num_neurons[2];
        int num_neurons_bias[2];

        double WTA_network[2];
        
        double *potential[num_city];
        double *threshold[num_city];
        double *last_spk[num_city];    // Used for weight_update
        double *last_spk_st[num_city]; // Used for SI_PAUSE in neuron
        double *last_spk_in[num_city]; // Used for IN_PAUSE in neuron


        // Synapse
        vector<vector<struct sm_pcmcell>> weight_matrix;
        vector<vector<struct sm_pcmcell>> weight_matrix_ideal;

        vector<vector<struct sm_pcmcell>> max_weight;
        vector<vector<struct sm_pcmcell>> min_weight;
        
        /* bipolar use this for both set and reset */
        vector<vector<struct sm_pcmcell>> wt_delta_g_set;
        vector<vector<struct sm_pcmcell>> wt_delta_g_reset;
        
        double *wsum[2];


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

    public:
        core();

    private:
        void assemble_prosign();
        void prosign_load();
        // NOT YET DEFINED Functions


    /*
        inline void ext_spike_load(char *fext, char *ftime);
        void weight_load(int cell_type, char *fweight);

        // Potential-update-related methods: implemented in sm_potential.cpp
        void potential_update_by_random_walk();
        template<int side> void potential_update_by_random_walk_core();

        void potential_update_by_leak(double tdiff);
        template<int side> void potential_update_by_leak_core(double tdiff);

        void potential_update_by_spk(sm_spk &spk_now, int which_spk);
        template<int side> void potential_update_by_spk_core(sm_spk &spk_now, double *wsum);

        void potential_reset(sm_spk &spk_now);
        void last_spk_st_update(sm_spk &spk_now);

        int compare_threshold(double tnow);
    */

};

#endif