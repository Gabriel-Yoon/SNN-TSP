// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <strings.h>

#include "smsim.hpp"
#include "core.hpp"
#include "sm_rng.hpp"

using namespace std;

int core::compare_threshold(double tnow)
{
    sm_spk *new_spk = new sm_spk;
    sm_spk *new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;

    for (int h_idx = 0; h_idx < num_neurons_datalabel[side_h]; h_idx++)
    { // exclude bias neuron

        bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
        bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

        if (!compared)
            continue;

        if (not_in_ref)
        {
            cout << "new internal spike h " << h_idx << " " << potential[side_h][h_idx] << " " << threshold[side_h][h_idx] << endl;

            // ST_PAUSE is always selected for hidden neurons
            // No need to update lask_spk_in
            last_spk_st[side_h][h_idx] = tnow;

            new_spk->spk.push_back(make_pair(side_h, h_idx));
        }
    }

    double time_fire2 = tnow + param.delay_spikein2out;

    // Push a potential reset event to queue
    int spike_flag = 0;
    if (new_spk_reset->spk.size() != 0)
    {
        new_spk_reset->time = time_fire2;
        new_spk_reset->reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        spike_flag = 1;
    }
    else
    {
        delete new_spk_reset;
    }

    // Push spike to queue
    if (new_spk->spk.size() != 0)
    {
        int phase_now = phase->query_phase_d_or_m(tnow);
        if (!param.enable_gpgm)
        {
            new_spk->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        }
        else
        {
            if (phase_now == sm_data_phase)
            {
                new_spk->time = time_fire2 + param.delay_spikeout2wlr_data + param.wlr_width;
            }
            else
            {
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
    }
    else
    {
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