// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <strings.h>

#include "smsim.hpp"
#include "core.hpp"
#include "sm_rng.hpp"

using namespace std;

template<int side> void core::potential_update_by_spk_core(sm_spk &spk_now, double *wsum) {
    double last_spk_refractory = spk_now.time - param.refractory_time;
    if(last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for(int idx = 0; idx < num_neurons[side]; idx++) {
        bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
        bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
        if(not_in_ref_st && (!param.hw_ISO_MOD || not_int_ref_in))
            potential[side][idx] += wsum[side][idx] * param.pt.alpha;
    }
}

void core::potential_update_by_spk(sm_spk &spk_now) {

    // Clear out wsum
    bzero((void*)wsum[side_v], sizeof(double) * num_neurons[side_v]);
    bzero((void*)wsum[side_h], sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        if(it->first == side_v) {
            assert(it->second < num_neurons[side_v]);
            spk_side_v = 1;
            int a = it->second / 5 + 1;
            int b = it->second % 5 + 1;
            printf('spike at WTA[%d][%d]\n', a, b);

            for(int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }
        } else { // side_h
            assert(it->second < num_neurons[side_h]);
            spk_side_h = 1;
            int a = it->second / 5 + 1;
            int b = it->second % 5 + 1;
            printf('spike at WTA[%d][%d]\n', a, b);

            for(int i = 0; i < num_neurons[side_v]; i++) {
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

int core::compare_threshold(double tnow)
{
    sm_spk *new_spk = new sm_spk;
    sm_spk *new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;
    int which_side = side;

    for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++){
        bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
        bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

        if(!compared) continue;
        
        // Create reset spike event
        if(param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
        }

        // Spike event checkboard 'last_spk_st' mark / spike event
        if(not_in_ref) {
            last_spk_st[side_h][h_idx] = tnow;
            new_spk->spk.push_back(make_pair(side_h, h_idx))
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
        new_spk->time = time_fire2 + param.delay_scanchain + param.wlr_width;

        queue_spk.push(make_pair(new_spk->time, new_spk));
        spike_flag = 1;

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