// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <strings.h>

#include "smsim.hpp"
#include "core.hpp"
#include "sm_rng.hpp"

using namespace std;

void core::potential_update_by_prosign()

template<int side> void core::potential_update_by_leak_core(double tdiff) {
    double exp_val = exp(-tdiff / param.pt_lk_tau);
    for(int i = 0; i < num_neurons_datalabel[side]; i++) {
        potential[side][i] *= exp_val;
    }

}

void core::potential_update_by_leak(double fdiff) {
    potential_update_by_leak_core<side_h>(tdiff);
    potential_update_by_leak_core<side_v>(tdiff);
}

template<int side> void core::potential_update_by_spk_core(sm_spk &spk_now, double *wsum) {
    double last_spk_refractory = spk_now.time - param.refractory_time;
    if(last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for(int idx = 0; idx < num_neurons_datalabel[side]; idx++) {
        if(!param.enable_ps2 || rng_ps2->get_val()) {
            bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
            bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
            if(not_in_ref_st && (!param.hw_ISO_MOD || not_int_ref_in))
                potential[side][idx] += wsum[idx] * param.pt_alpha;
        }
    }
}

void core::potential_update_by_spk(sm_spk &spk_now, int which_spk) {
    // bzero initialize wsum with 0 in the length of num_neurons
    bzero((void*)wsum[side_v], sizeof(double) * num_neurons_datalabel[side_v]);
    bzero((void*)wsum[side_h], sizeof(double) * num_neurons_datalabel[side_h]);
    
    int spk_side_v = 0;
    int spk_side_h = 0;
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        if(it->first == side_v) {
            assert(it->second < num_neurons[side_v]); //debug error check
            spk_side_v = 1;
            cout << "spike v " << it->second << endl;
            for(int i = 0; i < num_neurons_datalabel[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }
        } else { // side_h
            assert(it->second < num_neurons[side_h]);
            spk_side_h = 1;
            cout << "spike h " << it->second << endl;
            for(int i = 0; i < num_neurons_datalabel[side_v]; i++) {
                wsum[side_v][i] += (weight_matrix[i][it->second].Gp - weight_matrix[i][it->second].Gm);
            }
        }
    }

    if(spk_side_h) potential_update_by_spk_core<side_v>(spk_now, wsum[side_v]);
    if(spk_side_v) potential_update_by_spk_core<side_h>(spk_now, wsum[side_h]);
}

void core::potential_reset(sm_spk &spk_now) {
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        potential[it->first][it->second] = param.pt_init;
    }
}

void core::last_spk_st_update(sm_spk &spk_now) {
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        last_spk_st[it->first][it->second] = spk_now.time;
    }
}

int core::compare_threshold(double tnow) {
    sm_spk *new_spk = new sm_spk;
    sm_spk *new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;

    for(int h_idx = 0; h_idx < num_neurons_datalabel[side_h]; h_idx++) { // exclude bias neuron
        if(param.enable_ps2 && rng_ps2->get_val()) continue;

        bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
        bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

        if(!compared) continue;

        // CAP_ISO = ST_PAUSE || (IN_PAUSE && hw_ISO_MOD)
        // ST_PAUSE == IN_PAUSE in hidden neruons (always internal)
        // ST_PAUSE = high while in refractory time by INT_SPK
        // RESET = (CAP_ISO_RE && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = (INT_SPK && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = INT_SPK && (RES_EN || !CAP_ISO || !hw_RES_BLK)
        // INT_SPK = compared
        // CAP_ISO = !not_in_ref
        if(param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
        }

        if(not_in_ref) {
#ifdef DEBUG_SPK
            cout << "new internal spike h " << h_idx << " " << potential[side_h][h_idx] << " " << threshold[side_h][h_idx] << endl;
#endif
            // ST_PAUSE is always selected for hidden neurons
            // No need to update lask_spk_in
            last_spk_st[side_h][h_idx] = tnow;

            new_spk->spk.push_back(make_pair(side_h, h_idx));
        }
    }
    
    // Select target visible neurons
    int v_idx_start = 0;
    int v_idx_end = num_neurons_datalabel[side_v];
    // Decide the neurons in  internal spike mode
    if(param.enable_learning) {
        if(phase->query_phase_d_or_m(tnow) == sm_data_phase) {
            //v_idx_start = 0;
            v_idx_end = 0;
        } else {
            //v_idx_start = 0;
            //v_idx_end = param.num_neurons[side_V] - param.neurons_bias;
        }
    } else {
        if(param.inference_by_label) {
            //v_idx_start = 0;
            v_idx_end = neurons_visible_data; // NUM_NRN_I_DATA
        } else {
            v_idx_start = neurons_visible_data;
            //v_idx_end = param.num_neurons[side_v] - param.neurons_bias;
        }
    }

    for(int v_idx = 0; v_idx < num_neurons_datalabel[side_v]; v_idx++) {
        if(param.enable_ps2 && rng_ps2->get_val()) continue;

        bool compared = (potential[side_v][v_idx] > threshold[side_v][v_idx]);
        bool not_in_ref = (last_spk_st[side_v][v_idx] < time_in_ref);
        bool not_in_ref_in = (last_spk_in[side_v][v_idx] < time_in_ref);

        if(!compared) continue;

        if(not_in_ref_in) {
            last_spk_in[side_v][v_idx] = tnow;
        }

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

        if(not_in_ref) {
#ifdef DEBUG_SPK
            cout << "new internal spike v " << v_idx << " " << potential[side_v][v_idx] 
            << " " << threshold[side_v][v_idx] << endl;
#endif
            last_spk_st[side_v][v_idx] = tnow;

            new_spk->spk.push_back(make_pair(side_v, v_idx));
        }
    }

    double time_fire2 = tnow + param.delay_spikein2out;

    // Push a potential reset event to queue
    int spike_flag = 0;
    if(new_spk_reset->spk.size() != 0) {
        new_spk_reset->time = time_fire2;
        new_spk_reset->reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        spike_flag = 1;
    } else {
        delete new_spk_reset;
    }

    // Push spike to queue
    if(new_spk->spk.size() != 0) {
        int phase_now = phase->query_phase_d_or_m(tnow);
        if(!param.enable_gpgm) {
            new_spk->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        } else {
            if(phase_now == sm_data_phase) {
                new_spk->time = time_fire2 + param.delay_spikeout2wlr_data + param.wlr_width;
            } else {
                new_spk->time = time_fire2 + param.delay_spikeout2wlr_model + param.wlr_width;
            }
        }
        queue_spk.push(make_pair(new_spk->time, new_spk));
        spike_flag = 1;
#ifdef DEBUG_SPK
        cout << "new spike time " << new_spk->time << endl;
#endif

        // Dummy event to kick compare_threshold at the end of refractory time
        sm_spk *spk_ref = new sm_spk;
        spk_ref->time = tnow + param.refractory_time + FLOAT_EPSILON_TIME;
        queue_spk.push(make_pair(spk_ref->time, spk_ref));

        
    } else {
        delete new_spk;
    }

    return spike_flag;
}

/*
template<int side> void sm_core::potential_update_by_random_walk_core() {

    int start_neuron = 0;
    if(!param.enable_learning && (side == side_v)) {
        start_neuron = neurons_visible_data;
    }

#ifndef ENABLE_AVX
    for(int i  = start_neuron; i < num_neurons_datalabel[side]; i++) {
        int val = rng_rwalk->get_val() * 2 - 1; // 1 or -1
        potential[side][i] += (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val);
    }
#else
    int interval = sizeof(__m256d) / sizeof(double);
    int start_simd = (start_neuron % interval == 0) ? start_neuron : ((int)(start_neuron / interval) + 1) * interval;
    int end_simd = (num_neurons_datalabel[side] % interval == 0) ? num_neurons_datalabel[side]
                                        : (int)(num_neurons_datalabel[side] / interval) * interval;

    for(int i = start_neuron; i < start_simd; i++) {
        int val = rng_walk->get_val() * 2 - 1; // 1 or -1
        potential[side][i] += (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val);
    }

    __m256d veczero = _mm256_setzero_pd();
    __m256d step_plus = _mm256_set1_pd(param.random_walk_step * (1.0 + param.random_walk_mismatch));
    __m256d step_minus = _mm256_set1_pd(-param.random_walk_step * (1.0 - param.random_walk_mismatch));
    for(int i = start_simd; i < end_simd; i += interval) {
        __m256 rng_val = _mm256_set_pd(rng_rwalk->get_val(),
                                        rng_rwalk->get_val(),
                                        rng_rwalk->get_val(),
                                        rng_rwalk->get_val());
        __m256d pt = _mm256_load_pd(&potential[side][i]);
        __m256d val = _mm256_blendv_pd(step_plus, step_minus, _mm256_cmp_pd(rng_val, veczero, _CMP_EQ_OQ));
        _mm256_store_pd(&potential[side][i], _mm256_add_pd(pt, val));
    }

    for(int i = end_simd; i < num_neurons_datalabel[side]; i++) {
        int val = rng_rwalk->get_val() * 2 - 1; // 1 or -1
        potential[side][i] += (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val);
    }
#endif

}

void sm_core::potential_update_by_random_walk() {
    SPK_DUMP_H("r");

    potential_update_by_random_walk_core<side_h>();
    potential_update_by_random_walk_core<side_v>();
}
*/