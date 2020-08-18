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
        int h_WTA = idx / num_city + 1;
        int h_city = idx % num_city + 1;

        bool not_in_ref_st = last_spk_st[side][idx] < last_spk_refractory;
        bool not_in_ref_in = last_spk_in[side][idx] < last_spk_refractory;
        bool not_in_cap_iso = WTA[h_WTA][h_city].iso;
        if (not_in_ref_st && (!param.hw_ISO_MOD || not_in_ref_in) && not_in_cap_iso) {
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
        else {
            //cout << "EXCEPTION AT : " << idx + 1 << endl;
        }

    }
}

void core::potential_update_by_spk(sm_spk &spk_now) {

    //cout << "- <Start> Potential Update by Spike" << endl;

    // Clear out wsum
    memset(wsum[side_v], 0, sizeof(double) * num_neurons[side_v]);
    memset(wsum[side_h], 0, sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;
        
        for (int i = 1; i < num_city; i++) {
            WTA[i][h_city].iso = true;
        }

        if(it->first == side_v) { // This only happens for external spike at city 1.
            
            spk_side_v = 1;

            if (WTA[h_WTA][h_city].route == false) {
                WTA[h_WTA][h_city].route = true;
                //printf("WTA[%d][%d] was false but turned on!\n", h_WTA, h_city);
            }

            //printf("[EXT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);


            for(int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }

        } else { // side_h
            // This stands for internal spikes!

            spk_side_v = 1;

            if (WTA[h_WTA][h_city].route == false) {
                WTA[h_WTA][h_city].route = true;
                //printf("WTA[%d][%d] was false but turned on!\n", h_WTA, h_city);
            }

            //printf("[INT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);

            for(int i = 0; i < num_neurons[side_v]; i++) {
                wsum[side_h][i] += (weight_matrix[it->second][i].Gp - weight_matrix[it->second][i].Gm);
            }
            
        }

        for (int wta_num = 1; wta_num < num_city; wta_num++) {
            if (WTA[wta_num][h_city].iso == true) {
                wsum[side_h][num_city * (wta_num - 1) + (h_city - 1)] = 0;
            }
        }
        
    }

    // Set other neurons in Constraint WTA to be 0
    for (int i = 0; i < num_city; i++) {
        potential[side_h][i] = param.pt_init;
    }


    if(spk_side_h) potential_update_by_spk_core<side_v>(spk_now, wsum[side_v]);
    if(spk_side_v) potential_update_by_spk_core<side_h>(spk_now, wsum[side_h]);

    //cout << "- <End> Potential Update by Spike\n" << endl;

}

void core::potential_reset(sm_spk &spk_now) {
    
    //cout << "- <Start> Potential Reset" << endl;
    
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        
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

void core::last_spk_st_update(sm_spk &spk_now) {
    for(auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;
        
        last_spk_st[it->first][it->second] = spk_now.time;
        
        for (int i = 1; i < num_city + 1; i++) {
            WTA[i][h_city].iso = false;
        }

    }
}

int core::compare_threshold(double tnow){
    
    //cout << "- <Start> Compare Threshold" << endl;

    sm_spk *new_spk = new sm_spk;
    sm_spk *new_spk_reset = new sm_spk;
    double time_in_ref = tnow - param.refractory_time;
    bool two_spikes_in_one_WTA = false;
    int WTA_condition_checker = 0;
    int previous_route_city = 0;

    for (int i = 1; i < num_city + 1; i++) {
        for (int j = 1; j < num_city + 1; j++) {
            if (two_spikes_in_one_WTA) continue;
            
            int h_idx = (i-1) * num_city + j - 1;

            bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
            bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

            int h_WTA = i;
            int h_city = j;

            if (compared && not_in_ref && WTA[i][j].iso != true) {

                //printf("Potential over threshold at WTA[%d][%d]\n", h_WTA, h_city);                              

                for (int idx = 1; idx < num_city + 1; idx++) {
                    if (WTA[h_WTA][idx].route) {
                        previous_route_city = idx;
                        WTA_condition_checker++;
                    }
                }
                //printf("************************************************************\n");
                if (WTA_condition_checker == 0) {
                    //printf("New travel route at step %d\n", h_WTA);
                    WTA[h_WTA][h_city].route = true;
                }
                else if (WTA_condition_checker == 1) {
                    //printf("Modify travel route at step %d from city %d to city %d\n", h_WTA, previous_route_city, h_city);
                    WTA[h_WTA][previous_route_city].route = false;
                    WTA[h_WTA][h_city].route = true;
                }
                else {
                    printf("WTA condition error at step %d !!\n", h_WTA);
                }

                // Initialize the checker and the previous route city
                WTA_condition_checker = 0;
                previous_route_city = 0;
                two_spikes_in_one_WTA = true;
            }

            if (!compared) continue;
            
            // Create reset spike event
            if (param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
                new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
            }

            // Spike event checkboard 'last_spk_st' mark / spike event
            if (not_in_ref) {
                last_spk_st[side_h][h_idx] = tnow;
                //cout << "ST UPDATE COMPLETE" << endl;
                new_spk->spk.push_back(make_pair(side_h, h_idx));
            }

            WTA_condition_checker = 0;
            previous_route_city = 0;

        }
        two_spikes_in_one_WTA = false;
    }

    // Hidden side would only be considered
    for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++){
        
    }

    double time_fire2 = tnow + param.delay_spikein2out;

    // Push a potential reset event to queue
    int spike_flag = 0;

    if(new_spk_reset->spk.size() != 0) {
        new_spk_reset->time = time_fire2;
        new_spk_reset->reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        //cout << "Reset event loaded to queue_spk at : " << new_spk_reset->time << endl;
        spike_flag = 1;
    } else {
        delete new_spk_reset;
    }

    // Push spike to queue
    if(new_spk->spk.size() != 0) {
        new_spk->time = time_fire2 + param.delay_scanchain + param.wlr_width;

        queue_spk.push(make_pair(new_spk->time, new_spk));
        //cout << "Spike event loaded to queue_spk at : " << new_spk->time << endl;
        spike_flag = 1;

        // Dummy event to kick compare_threshold at the end of refractory time
        sm_spk *spk_ref = new sm_spk;
        spk_ref->time = tnow + param.refractory_time + FLOAT_EPSILON_TIME;
        queue_spk.push(make_pair(spk_ref->time, spk_ref));
    } else {
        delete new_spk;
    }

    //cout << "- <End> Compare Threshold\n" << endl;
    if (spike_flag) {
        //cout << " -------------New spike detected!------------- \n" << endl;
    }
    return spike_flag;
}

template<int side> void core::potential_update_by_random_walk_core(double tnow) {

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
        
        double last_spk_refractory = tnow - param.refractory_time;
        bool not_in_ref_st = last_spk_st[side][i] < last_spk_refractory;

        // neurons in refractory state goes zero
        if (!not_in_ref_st) {
            potential[side][i] = 0;
        }

        for (int i = 0; i < num_city; i++) {
            potential[side_h][i] = param.pt_init;
        }

        //printf("Potential of WTA[%d][%d] changed by %lf \n", h_WTA, h_city, (param.random_walk_step * val) * (1 + param.random_walk_mismatch * val));
        
    }
    
}

void core::potential_update_by_random_walk(double tnow) {
    
    //cout << "- <Start> Random Walk" << endl;

    potential_update_by_random_walk_core<side_h>(tnow);
    //potential_update_by_random_walk_core<side_v>();

    //cout << "- <End> Random Walk\n" << endl;
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