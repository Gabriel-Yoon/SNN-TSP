// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <math.h>
#include <random>
#include <chrono>
#include <string.h>

#include "spike.hpp"
#include "core.hpp"
#include "synapse.hpp"

using namespace std;

template<int side> void core::potential_update_by_spk_core(sm_spk& spk_now, double* wsum) {
    double last_spk_refractory = spk_now.time - param.refractory_time;
    if (last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for (int idx = 0; idx < num_neurons[side]; idx++) {
        int h_WTA = idx / num_city + 1;
        int h_city = idx % num_city + 1;

        bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
        bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
        bool not_in_cap_iso = !WTA[h_WTA][h_city].iso;

        if (not_in_ref_st && (!param.hw_ISO_MOD || not_in_ref_in) && not_in_cap_iso) {

            if (wsum[idx] * param.pt_alpha != 0) {
                // printf("Potential of WTA[%d][%d] rose by %lf \n", h_WTA, h_city, wsum[idx] * param.pt_alpha);
            }
            if (potential[side][idx] + wsum[idx] * param.pt_alpha <= 0) {
                potential[side][idx] = 0;
            }
            else {
                potential[side][idx] += wsum[idx] * param.pt_alpha;
            }
        }
        else {
            //cout << "EXCEPTION AT : " << idx + 1 << endl;
        }

    }
}

void core::potential_update_by_spk(sm_spk& spk_now) {

    //cout << "- <Start> Potential Update by Spike" << endl;

    // Clear out wsum
    memset(wsum[side_v], 0, sizeof(double) * num_neurons[side_v]);
    memset(wsum[side_h], 0, sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;

    // Caution : do not use continue option! This it stands for each spike infos.
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        if (it->first == side_v) { // External spike at city 1 (only)

            spk_side_v = 1;

            //printf("[EXT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);

            for (int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }

        }
        else { // side_h
         // This stands for internal spikes!

            spk_side_v = 1;

            //printf("[INT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);

            for (int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }

        }

    }

    if (spk_side_h) potential_update_by_spk_core<side_v>(spk_now, wsum[side_v]);
    if (spk_side_v) potential_update_by_spk_core<side_h>(spk_now, wsum[side_h]);

    //cout << "- <End> Potential Update by Spike\n" << endl;

}

void core::potential_reset(sm_spk& spk_now) {

    //cout << "- <Start> Potential Reset" << endl;

    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

        // For Constraint.. external spike at 1,1
        if (it->first == side_v && it->second == 0) {
            potential[side_h][it->second] = param.pt_init;
        }
        else {
            potential[side_h][it->second] = param.pt_init;
        }

        //printf("Potential reset on WTA[%d][%d]\n", it->second / num_city + 1, it->second % num_city + 1);
    }
    //cout << "- <End> Potential Reset\n" << endl;

}

void core::last_spk_st_update(sm_spk& spk_now) {
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        last_spk_st[it->first][it->second] = spk_now.time;
    }
}

void core::potential_reset(sm_spk& spk_now) {
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        potential[it->first][it->second] = param.pt_init;
    }
}

void core::last_spk_st_update(sm_spk& spk_now) {
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        last_spk_st[it->first][it->second] = spk_now.time;
    }
}

int core::compare_threshold(double tnow) {
    sm_spk* new_spk = new sm_spk;
    sm_spk* new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;

    for (int h_idx = 0; h_idx < num_neurons_datalabel[side_h]; h_idx++) { // exclude bias neuron
        if (param.enable_ps2 && rng_ps2->get_val()) continue;

        bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
        bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

        if (!compared) continue;

        // CAP_ISO = ST_PAUSE || (IN_PAUSE && hw_ISO_MOD)
        // ST_PAUSE == IN_PAUSE in hidden neruons (always internal)
        // ST_PAUSE = high while in refractory time by INT_SPK
        // RESET = (CAP_ISO_RE && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = (INT_SPK && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = INT_SPK && (RES_EN || !CAP_ISO || !hw_RES_BLK)
        // INT_SPK = compared
        // CAP_ISO = !not_in_ref
        if (param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
        }
        // Create spike event
        // 4. Mark spike time into the last_spk_st board. no **last_spk_update!
        // Spike event checkboard 'last_spk_st' mark
        if (not_in_ref) {
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
    if (param.enable_learning) {
        if (phase->query_phase_d_or_m(tnow) == sm_data_phase) {
            //v_idx_start = 0;
            v_idx_end = 0;
        }
        else {
            //v_idx_start = 0;
            //v_idx_end = param.num_neurons[side_V] - param.neurons_bias;
        }
    }
    else {
        if (param.inference_by_label) {
            //v_idx_start = 0;
            v_idx_end = neurons_visible_data; // NUM_NRN_I_DATA
        }
        else {
            v_idx_start = neurons_visible_data;
            //v_idx_end = param.num_neurons[side_v] - param.neurons_bias;
        }
    }

    for (int v_idx = 0; v_idx < num_neurons_datalabel[side_v]; v_idx++) {
        if (param.enable_ps2 && rng_ps2->get_val()) continue;

        bool compared = (potential[side_v][v_idx] > threshold[side_v][v_idx]);
        bool not_in_ref = (last_spk_st[side_v][v_idx] < time_in_ref);
        bool not_in_ref_in = (last_spk_in[side_v][v_idx] < time_in_ref);

        if (!compared) continue;

        if (not_in_ref_in) {
            last_spk_in[side_v][v_idx] = tnow;
        }

        // Neurons in external spike mode
        if ((v_idx < v_idx_start) || (v_idx >= v_idx_end)) {
            //Potential reset by external spikes is handled in potential_update_by_spk()
            bool cap_iso = !not_in_ref || (!not_in_ref_in && param.hw_ISO_MOD);
            bool res_en_int = param.hw_RES_EN && not_in_ref_in && param.hw_ISO_MOD;
            bool res_blk = param.hw_RES_BLK && cap_iso;

            if (res_en_int || !res_blk) {
                new_spk_reset->spk.push_back(make_pair(side_v, v_idx));
            }
            continue;
        }

        //Neurons in internal spike mode
        // RESET = (CAP_ISO_RE && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = (INT_SPK && RES_EN) || (INT_SPK && (!(CAP_ISO && hw_RES_BLK)))
        //       = INT_SPK && (RES_EN || !CAP_ISO || !hw_RES_BLK)
        if (param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
            new_spk_reset->spk.push_back(make_pair(side_v, v_idx));
        }

        if (not_in_ref) {
                last_spk_st[side_v][v_idx] = tnow;

            new_spk->spk.push_back(make_pair(side_v, v_idx));
        }
    }

    double time_fire2 = tnow + param.delay_spikein2out;

    // Push a potential reset event to queue
    int spike_flag = 0;
    if (new_spk_reset->spk.size() != 0) {
        new_spk_reset->time = time_fire2;
        new_spk_reset->reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        spike_flag = 1;
    }
    else {
        delete new_spk_reset;
    }

    // Push spike to queue
    if (new_spk->spk.size() != 0) {
        int phase_now = phase->query_phase_d_or_m(tnow);
        if (!param.enable_gpgm) {
            new_spk->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        }
        else {
            if (phase_now == sm_data_phase) {
                new_spk->time = time_fire2 + param.delay_spikeout2wlr_data + param.wlr_width;
            }
            else {
                new_spk->time = time_fire2 + param.delay_spikeout2wlr_model + param.wlr_width;
            }
        }
        queue_spk.push(make_pair(new_spk->time, new_spk));
        spike_flag = 1;
#ifdef DEBUG_SPK
        cout << "new spike time " << new_spk->time << endl;
#endif

        // Dummy event to kick compare_threshold at the end of refractory time
        sm_spk* spk_ref = new sm_spk;
        spk_ref->time = tnow + param.refractory_time + FLOAT_EPSILON_TIME;
        queue_spk.push(make_pair(spk_ref->time, spk_ref));

        // WUP event
        if (param.enable_learning) {
            if (!param.enable_gpgm) {
                sm_spk* new_spk_wup = new sm_spk(*new_spk); // WUP event
                new_spk_wup->time = time_fire2 + param.delay_spikeout2wup;
                queue_wup_spk.push(make_pair(new_spk_wup->time, new_spk_wup));
            }
            else {
                sm_spk* new_spk_wup_v = new sm_spk;
                sm_spk* new_spk_wup_h = new sm_spk;
                for (auto it = new_spk->spk.begin(); it != new_spk->spk.end(); it++) {
                    if (it->first == side_v) {
                        new_spk_wup_v->spk.push_back(*it);
                    }
                    else {
                        new_spk_wup_h->spk.push_back(*it);
                    }
                }
                if (new_spk_wup_v->spk.size() > 0) {
                    if (phase_now == sm_data_phase) {
                        new_spk_wup_v->time = time_fire2 + param.delay_spikeout2td3;
                    }
                    else {
                        new_spk_wup_v->time = time_fire2 + param.delay_spikeout2td3 - param.tset_width;
                    }
                    queue_wup_spk.push(make_pair(new_spk_wup_v->time, new_spk_wup_v));
                }
                else {
                    delete new_spk_wup_v;
                }
                if (new_spk_wup_h->spk.size() > 0) {
                    if (phase_now == sm_data_phase) {
                        new_spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_data;
                    }
                    else {
                        new_spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_model;
                    }
                    queue_wup_spk.push(make_pair(new_spk_wup_h->time, new_spk_wup_h));
                }
                else {
                    delete new_spk_wup_h;
                }
            }
        }
    }
    else {
        delete new_spk;
    }
    // spike_flag is 1 if there is new spike
    return spike_flag;
}

template<int side> void core::potential_update_by_random_walk_core(double tnow) {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(/*average value*/ 0, /*standard deviation*/ 1);

    int val = 0;

    for (int i = 0; i < num_neurons[side]; i++) {

        int h_WTA = i / num_city + 1;
        int h_city = i % num_city + 1;
        double last_spk_refractory = tnow - param.refractory_time;
        bool not_in_ref_st = last_spk_st[side][i] < last_spk_refractory;

        if (param.hw_CAP_ISO && WTA[h_WTA][h_city].iso) continue;
        if (!not_in_ref_st) continue;

        if (dist(generator) < 0) {
            val = -1;
        }
        else {
            val = 1;
        }

        if (potential[side][i] + (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val) <= 0) {
            potential[side][i] = 0;
        }
        else {
            potential[side][i] += (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val);
        }

        //printf("Potential of WTA[%d][%d] changed by %lf \n", h_WTA, h_city, (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val));

    }

}

void core::instantaneous_firing_probability() {

}

void core::potential_update_by_random_walk(double tnow) {

    //cout << "- <Start> Random Walk" << endl;

    potential_update_by_random_walk_core<side_h>(tnow);
    //potential_update_by_random_walk_core<side_v>();

    //cout << "- <End> Random Walk\n" << endl;
}

void core::random_walk_annealing_schedule(double tnow, double tend) {
    param.random_walk_step = 0.03 + (0.06-0.03) * exp(-tnow / 20);
}

template<int side> void core::potential_update_by_leak_core(double tdiff) {

    double exp_val = exp(-tdiff / param.pt_lk_tau);
    for (int i = 0; i < num_neurons[side]; i++) {
        potential[side][i] *= exp_val;
    }

}

void core::potential_update_by_leak(double tdiff) {

    //cout << "- <Start> Leak" << endl;

    potential_update_by_leak_core<side_h>(tdiff);
    //potential_update_by_leak_core<side_v>(tdiff);

    //cout << "- <End> Leak\n" << endl;
}