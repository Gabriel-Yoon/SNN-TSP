// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <math.h>

#include "spike_control.hpp"
#include "core.hpp"


using namespace std;

template<int side> void core::potential_update_by_spk_core(sm_spk &spk_now, double *wsum) {
    double last_spk_refractory = spk_now.time - param.refractory_time;
    if(last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for(int idx = 0; idx < num_neurons[side]; idx++) {
        bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
        bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
        if(not_in_ref_st && (!param.hw_ISO_MOD || not_in_ref_in))
            potential[side][idx] += wsum[side][idx] * param.pt_alpha;
    }
}

void core::potential_update_by_spk(sm_spk &spk_now, int which_spk) {

    // Clear out wsum
    memset((void*)wsum[side_v], 0, sizeof(double) * num_neurons[side_v]);
    memset((void*)wsum[side_h], 0, sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        if(it->first == side_v) {
            assert(it->second < num_neurons[side_v]);
            spk_side_v = 1;
            int a = it->second / 5 + 1;
            int b = it->second % 5 + 1;
            printf("spike at WTA[%d][%d]\n", a, b);

            for(int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }
        } else { // side_h
            assert(it->second < num_neurons[side_h]);
            spk_side_h = 1;
            int a = it->second / 5 + 1;
            int b = it->second % 5 + 1;
            printf("spike at WTA[%d][%d]\n", a, b);

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
            new_spk->spk.push_back(make_pair(side_h, h_idx));
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

template<int side> void core::potential_update_by_random_walk_core() {

    for (int i = 0; i < num_neurons[side]; i++) {
        int val = 1; // 1 or -1
        potential[side][i] += (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val);
    }

}

void core::potential_update_by_random_walk() {
    potential_update_by_random_walk_core<side_h>();
    potential_update_by_random_walk_core<side_v>();
}



template<int side> void core::potential_update_by_leak_core(double tdiff) {

    double exp_val = exp(-tdiff / param.pt_lk_tau);
    for (int i = 0; i < num_neurons[side]; i++) {
        potential[side][i] *= exp_val;
    }

}

void core::potential_update_by_leak(double tdiff) {
    potential_update_by_leak_core<side_h>(tdiff);
    potential_update_by_leak_core<side_v>(tdiff);
}


void core::ext_spike_load(double tend) {

    int side;
    int neuron = 0;
    double time;
    double injection_step = fmod(tend, param.timestep_injection);
    priority_queue<pair<double, sm_spk_one*>, vector<pair<double, sm_spk_one*>>,greater<pair<double, sm_spk_one*>>> queue_ext_pri;

    for (int i = 0; i < injection_step; i++) {
        sm_spk_one* spk = new sm_spk_one;
        spk->time = i * param.timestep_injection;
        spk->side = side_h;
        spk->neuron = neuron;
        queue_ext_pri.push(make_pair(time, spk));
    }
  
    while (!queue_ext_pri.empty()) {
        sm_spk_one* spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        // Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
        sm_spk* spk_ref = new sm_spk;
        spk_ref->time = ref_end_time;
        queue_ext.push(make_pair(ref_end_time, spk_ref));

        sm_spk* spk_ext = new sm_spk; // Actual spike event
        spk_ext->spk.push_back(make_pair(spk_one->side, spk_one->neuron));

        //Find spikes at the same timing
        while (!queue_ext_pri.empty()) {
            sm_spk_one* spk_next = queue_ext_pri.top().second;
            if (spk_one->time != spk_next->time) { break; }
            spk_ext->spk.push_back(make_pair(spk_next->side, spk_next->neuron));
            delete spk_next;
            queue_ext_pri.pop();
        }

        // Create last_spk_st update event for ST_PAUSE
        sm_spk* spk_st_update = new sm_spk(*spk_ext);
        spk_st_update->time = spk_one->time;
        spk_st_update->st = true;
        queue_ext.push(make_pair(spk_st_update->time, spk_st_update));

        double time_fire2 = spk_one->time + param.delay_spikein2out;

        // Create reset event
        if (param.hw_RES_EN) {
            sm_spk* spk_ext_reset = new sm_spk(*spk_ext);
            spk_ext_reset->time = time_fire2;
            spk_ext_reset->reset = true;
            queue_ext.push(make_pair(time_fire2, spk_ext_reset));
        }

        // Push spike event to queue

        spk_ext->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;

        queue_ext.push(make_pair(spk_ext->time, spk_ext));


        delete spk_one;
    }
}