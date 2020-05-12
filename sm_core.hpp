
#ifndef __SM_CORE_HPP__
#define __SM_CORE_HPP__

#include <vector>
#include <string>

#ifdef ENABLE_AVX
#include <x86intrin.h>
#endif

#include "smsim.hpp"
#include "sm_param.hpp"
#include "sm_rng.hpp

class sm_core {
    
    private:
        sm_param param;
        sm_rng_1bit *rng_rwalk;
        sm_rng_1bit *rng_ps2;
        sm_rng_ureal01 rng_vth;
        sm_rng_normal *rng_wt_set; // master & circuit
        sm_rng_normal *rngwt_reset; // circuit
        sm_rng_ureal01 *rng_wt_reset_rate; // circuit

        priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, 
                                    spk_cmp> queue_ext;
        priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, 
                                    spk_cmp> queue_spk;
        priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, 
                                    spk_cmp> queue_wup_ext;
        priority_queue<pair<double, sm_spk*>, vector<pair<double, sm_spk*>>, 
                                    spk_cmp> queue_wup_spk;

        vector<vector<struct sm_pcmcell>> weight_matrix;
        vector<vector<struct sm_pcmcell>> weight_matrix_ideal;
        vector<vector<struct sm_pcmcell>> max_weight;
        vector<vector<struct sm_pcmcell>> min_weight;
        vector<vector<struct sm_pcmcell>> wt_delta_g_set; // bipolar use this for both set and reset
        vector<vector<struct sm_pcmcell>> wt_delta_g_reset;

        double *potential[2];
        double *threshold[2];
        double *last_spk[2];    // Used for weight_update
        double *last_spk_st[2]; // Used for SI_PAUSE in neuron
        double *last_spk_in[2]; // Used for IN_PAUSE in neuron
        double *wsum[2];

        int neurons_visible_data;
        int neurons_visible_label;
        int neurons_visible_bias;
        int neurons_hidden_data;
        int neurons_hidden_bias;
        int num_neurons[2];
        int num_neurons_datalabel[2];
        int num_neurons_bias[2];
    
    public:
        sm_core(std::string param_file);

        void initialize(char* fextspk, char* fexttime, char* fwij, char* fwij_gp, char* fwij_gm);
        void print_params();
        void print();

        double run();

// int put_spk(sm_spk *spk_new);

    private:
        int get_spk(sm_spk **spk_now, int *which_spk);
        void ext_spike_load(char *fext, char *ftime);
        void weight_load(int cell_type, char *fweight);
        void weight_save(int cell_type, string filename);

        void initialize_export();
        void export_spk(sm_spk &spk_now, int which_spk);
        void export_ptn(double tnow);
        void close_export();

        template<int is_spk, int is_rng>
        void run_loop(double tnow, double tpre, sm_spk &spk_now, int which_spk, double &simtick, int &new_spk);

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
        
        void threshold_update_stochstic();

        // Weight update method: implemented in sm_weight.cpp
        void weight_update(double spk_now_time);
        void weight_update_bipolar(int phase, int side, int spk_idx);
        void weight_update_gpgm(int phase, int side, int spk_idx, double wup_time);
        inline void weight_set_gp(int v_idx, int h_idx);
        inline void weight_set_gm(int v_idx, int h_idx);
        inline void weight_reset_gp(int v_idx, int h_idx);
        inline void weight_reset_gm(int v_idx, int h_idx);

};

#endif // __SM_CORE_HPP