// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <math.h>
#include <random>
#include <chrono>

#include "spike_control.hpp"
#include "core.hpp"


using namespace std;

template<int side> void core::potential_update_by_spk_core(sm_spk &spk_now, double *wsum) {
    double last_spk_refractory = spk_now.time - param.refractory_time;
    if(last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for(int idx = 0; idx < num_neurons[side]; idx++) {
        bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
        bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
        if (not_in_ref_st && (!param.hw_ISO_MOD || not_in_ref_in)) {
            int h_WTA = idx / num_city + 1;
            int h_city = idx % num_city + 1;
            if (wsum[idx] * param.pt_alpha != 0) {
                //printf("Potential of WTA[%d][%d] rose by %lf \n", h_WTA, h_city, wsum[idx] * param.pt_alpha);
            }
            if (potential[side][idx] + wsum[idx] * param.pt_alpha <= 0) {
                potential[side][idx] = 0;
            }
            else {
                potential[side][idx] += wsum[idx] * param.pt_alpha;
            }
        }
    }
}

void core::potential_update_by_spk(sm_spk &spk_now) {

    cout << "- <Start> Potential Update by Spike" << endl;

    // Clear out wsum
    memset(wsum[side_v], 0, sizeof(double) * num_neurons[side_v]);
    memset(wsum[side_h], 0, sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        if(it->first == side_v) {
            
            spk_side_v = 1;
            int a = it->second / num_city + 1;
            int b = it->second % num_city + 1;
            printf("spike is generating at WTA[%d][%d]\n", a, b);

            for(int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }
        } /*else { // side_h
            
            spk_side_h = 1;
            int a = it->second / 5 + 1;
            int b = it->second % 5 + 1;
            printf("spike at WTA[%d][%d]\n", a, b);

            for(int i = 0; i < num_neurons[side_v]; i++) {
                wsum[side_v][i] += (weight_matrix[i][it->second].Gp - weight_matrix[i][it->second].Gm);
            }
            
        }
        */
    }

    if(spk_side_h) potential_update_by_spk_core<side_v>(spk_now, wsum[side_v]);
    if(spk_side_v) potential_update_by_spk_core<side_h>(spk_now, wsum[side_h]);

    cout << "- <End> Potential Update by Spike\n" << endl;

}

void core::potential_reset(sm_spk &spk_now) {
    
    cout << "- <Start> Potential Reset" << endl;
    
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        potential[it->first][it->second] = param.pt_init;
        printf("Potential reset on WTA[%d][%d]\n", it->second / num_city + 1, it->second % num_city + 1);
    }
    cout << "- <End> Potential Reset\n" << endl;

}

void core::last_spk_st_update(sm_spk &spk_now) {
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        last_spk_st[it->first][it->second] = spk_now.time;
    }
}

int core::compare_threshold(double tnow){
    
    cout << "- <Start> Compare Threshold" << endl;

    sm_spk *new_spk = new sm_spk;
    sm_spk *new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;

    // Hidden side would only be considered
    for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++){
        bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
        bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;
        
        int h_WTA = h_idx / num_city + 1;
        int h_city = h_idx % num_city + 1;
        
        if (compared) {

            printf("Potential over threshold at WTA[%d][%d]\n", h_WTA, h_city);

            int WTA_condition_checker = 0;
            int previous_route_city = 0;

            for (int idx = 1; idx < num_city+1; idx++) {
                if (WTA[h_WTA][idx]) {
                    previous_route_city = idx;
                    WTA_condition_checker++;
                }
            }
            printf("************************************************************\n");
            if (WTA_condition_checker == 0) {
                printf("New travel route at step %d\n", h_WTA);
                WTA[h_WTA][h_city] = true;
            }
            else if (WTA_condition_checker == 1) {
                printf("Modify travel route at step %d from city %d to city %d\n", h_WTA, previous_route_city, h_city);
                WTA[h_WTA][previous_route_city] = false;
                WTA[h_WTA][h_city] = true;
            }
            else {
                printf("WTA condition error at step %d !!\n", h_WTA);
            }
        }

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
        cout << "Reset event loaded to queue_spk at : " << new_spk_reset->time << endl;
        spike_flag = 1;
    } else {
        delete new_spk_reset;
    }

    // Push spike to queue
    if(new_spk->spk.size() != 0) {
        new_spk->time = time_fire2 + param.delay_scanchain + param.wlr_width;

        queue_spk.push(make_pair(new_spk->time, new_spk));
        cout << "Spike event loaded to queue_spk at : " << new_spk->time << endl;
        spike_flag = 1;

        // Dummy event to kick compare_threshold at the end of refractory time
        sm_spk *spk_ref = new sm_spk;
        spk_ref->time = tnow + param.refractory_time + FLOAT_EPSILON_TIME;
        queue_spk.push(make_pair(spk_ref->time, spk_ref));
    } else {
        delete new_spk;
    }

    cout << "- <End> Compare Threshold\n" << endl;
    if (spike_flag) {
        cout << " -------------New spike detected!------------- \n" << endl;
    }
    return spike_flag;
}

template<int side> void core::potential_update_by_random_walk_core() {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(/*average value*/ 0, /*standard deviation*/ 1);
    
    int val = 0;

    for (int i = 0; i < num_neurons[side]; i++) {
        
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
        int h_WTA = i / num_city + 1;
        int h_city = i % num_city + 1;
        
        //printf("Potential of WTA[%d][%d] changed by %lf \n", h_WTA, h_city, (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val));
        
    }

}

void core::potential_update_by_random_walk() {
    
    cout << "- <Start> Random Walk" << endl;
    
    potential_update_by_random_walk_core<side_h>();
    //potential_update_by_random_walk_core<side_v>();

    cout << "- <End> Random Walk\n" << endl;
}

template<int side> void core::potential_update_by_leak_core(double tdiff) {

    double exp_val = exp(-tdiff / param.pt_lk_tau);
    for (int i = 0; i < num_neurons[side]; i++) {
        potential[side][i] *= exp_val;
    }

}

void core::potential_update_by_leak(double tdiff) {
    
    cout << "- <Start> Leak" << endl;
    
    potential_update_by_leak_core<side_h>(tdiff);
    //potential_update_by_leak_core<side_v>(tdiff);

    cout << "- <End> Leak\n" << endl;
}