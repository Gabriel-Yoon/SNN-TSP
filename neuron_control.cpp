// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <math.h>
#include <random>
#include <chrono>

#include "spike_control.hpp"
#include "core.hpp"

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

void core::wta_condition_update(sm_spk& spk_now, double tnow, double tend) {

    // Q. How to handle overlapped case? There are two obstacles to handle.
    int previous_spike_within_WTA_index = -1;
    bool wta_condition_checker = false;

    // Caution : do not use continue option! This it stands for each spike infos.
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;
        wta_condition_checker = false;

        // Reset event & last_spk_st update should be avoided . Should I add WTA[i][j].iso == true condition?
        if (!(spk_now.st || spk_now.reset)) {

            // Dummy event of the end of refractory time
            if (spk_now.iso == -1) { //WTA[h_WTA][h_city].route == true
                WTA[h_WTA][h_city].route = false;
                // printf("OFF_WTA[%d][%d]", h_WTA, h_city);
                if (param.hw_CAP_ISO) {
                    cap_iso_update();
                }
            }
            
            // Real spike event
            else if (spk_now.iso == 1) { //WTA[h_WTA][h_city].route == false
                // First, check whether if there was a spike from the same WTA or other WTA of the same city.
                // Be aware of the spike itself.
                /*
                for (int i = 1; i < num_city; i++) {
                    
                    if (tnow - last_spk_st[side_h][(i - 1) * num_city + h_city-1] < param.refractory_time && it->second != (i - 1) * num_city + h_city-1) {
                        wta_condition_checker = true;
                    }
                    if (tnow - last_spk_st[side_h][(h_WTA - 1) * num_city + i-1] < param.refractory_time && it->second != (h_WTA - 1) * num_city + i-1) {
                        wta_condition_checker = true;
                    }
                }
                */

                for (int i = 1; i < num_city + 1; i++) {
                    if (WTA[h_WTA][i].route == true || WTA[i][h_city].route == true) {
                        wta_condition_checker = true;
                    }
                }

                if (wta_condition_checker == false) {
                    WTA[h_WTA][h_city].route = true;
                    if (tnow > (tend-1) && tnow < tend) {
                        spike_counter[h_WTA][h_city].num_spike++;
                        last_spike[h_WTA] = h_city;
                    }
                    // printf("ON_WTA[%d][%d]", h_WTA, h_city);
                    //previous_spike_within_WTA_index = h_WTA;
                    if (param.hw_CAP_ISO) {
                        cap_iso_update();
                    }
                }
                else { // WTA already working! spike won't be functioning. Just pass by
                   //printf("_________WTA error!________Error on WTA[%d][%d]", h_WTA, h_city);
                }
            }
            // until here
        }

        if (it->first == side_v) {
            /*
            // Clear out all the route and this city is owned exclusively
            for (int i = 1; i < num_city + 1; i++) {
                WTA[i][h_city].route = false;
                WTA[h_WTA][i].route = false;
            }
            */

            // WTA[h_WTA][h_city].route = true;
        }
        else { //side_h



            /*
            // Refractory Time ended -> route to false!
            // was if ((tnow - last_spk_st[it->first][it->second]) > param.refractory_time+ FLOAT_EPSILON_TIME)
            if(spk_now.iso){
                // Turn back to the original state (transition state)
                WTA[h_WTA][h_city].route = false;
            }

            // Newly generated spike
            //fabs(last_spk_st[it->first][it->second] - tnow) <= FLOAT_EPSILON_TIME
            else {

                // Clear out all the route and this city is owned exclusively
                for (int i = 1; i < num_city + 1; i++) {
                    WTA[i][h_city].route = false;
                    WTA[h_WTA][i].route = false;
                }


                WTA[h_WTA][h_city].route = true;
            }
            */

        }
    }
}

void core::cap_iso_update() { // This function works only when (param.hw_CAP_ISO) is true

    // Initialization of cap_iso profile
    for (int i = 1; i < num_city + 1; i++) {
        for (int j = 1; j < num_city + 1; j++) {
            WTA[i][j].iso = false;
        }
    }

    // Conduct cap_iso profile based on WTA.route
    for (int i = 1; i < num_city + 1; i++) {
        for (int j = 1; j < num_city + 1; j++) {
            if (WTA[i][j].route == true) {
                for (int k = 1; k < num_city + 1; k++) {
                    WTA[i][k].iso = true;
                    WTA[k][j].iso = true;
                }
            }
        }
    }

    /*
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        if (!param.hw_CAP_ISO) continue;

        // Spike event : iso on process proceed
        if (spk_now.iso) {
            for (int i = 1; i < num_city + 1; i++) {
                // Same city in other WTA network CAP_ISO
                WTA[i][h_city].iso = true;
                // Other city in the same WTA network CAP_ISO
                WTA[h_WTA][i].iso = true;
            }
        }
        // Refractory time end event : iso is turning off process proceed
        else {
            for (int i = 1; i < num_city + 1; i++) {
                // Same city in other WTA network CAP_ISO
                WTA[i][h_city].iso = false;
                // Other city in the same WTA network CAP_ISO
                WTA[h_WTA][i].iso = false;
            }
        }
    }

    */
}

void core::last_spk_st_update(sm_spk& spk_now) {
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        last_spk_st[it->first][it->second] = spk_now.time;
    }
}

int core::compare_threshold(double tnow) {

    //cout << "- <Start> Compare Threshold" << endl;

    sm_spk* new_spk = new sm_spk;
    sm_spk* new_spk_reset = new sm_spk;

    double time_in_ref = tnow - param.refractory_time;

    for (int i = 1; i < num_city + 1; i++) {
        for (int j = 1; j < num_city + 1; j++) {

            int h_idx = (i - 1) * num_city + j - 1;

            bool compared = potential[side_h][h_idx] > threshold[side_h][h_idx];
            bool not_in_ref = last_spk_st[side_h][h_idx] < time_in_ref;

            if (!compared) continue;

            // Create reset spike event
            if (param.hw_RES_EN || not_in_ref || !param.hw_RES_BLK) {
                new_spk_reset->spk.push_back(make_pair(side_h, h_idx));
            }

            // Create spike event
            // 4. Mark spike time into the last_spk_st board. no **last_spk_update!
            // Spike event checkboard 'last_spk_st' mark
            if (not_in_ref) {
                last_spk_st[side_h][h_idx] = tnow;
                //cout << "ST UPDATE COMPLETE" << endl;
                new_spk->spk.push_back(make_pair(side_h, h_idx));
            }
        }
    }

    double time_fire2 = tnow + param.delay_spikein2out;

    int spike_flag = 0;

    // 1. Push a potential reset event to queue
    if (new_spk_reset->spk.size() != 0) {
        new_spk_reset->time = time_fire2;
        new_spk_reset->reset = true;
        queue_spk.push(make_pair(time_fire2, new_spk_reset));
        //cout << "Reset event loaded to queue_spk at : " << new_spk_reset->time << endl;
        spike_flag = 1;

    }
    else {
        delete new_spk_reset;
    }

    if (new_spk->spk.size() != 0) {
        new_spk->time = time_fire2 + param.delay_scanchain + param.wlr_width;

        // 2. Push spike to queue
        new_spk->iso = 1;
        queue_spk.push(make_pair(new_spk->time, new_spk));

        spike_flag = 1;

        // 3. Dummy event to kick compare_threshold at the end of refractory time
        sm_spk* spk_ref = new sm_spk(*new_spk);
        spk_ref->time = tnow + param.refractory_time + FLOAT_EPSILON_TIME;
        spk_ref->iso = -1;
        queue_spk.push(make_pair(spk_ref->time, spk_ref));

    }
    else {
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

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0,1); //uniform distribution btw 0 and 1

    //if (dis(gen));

}

void core::potential_update_by_random_walk(double tnow) {

    //cout << "- <Start> Random Walk" << endl;

    potential_update_by_random_walk_core<side_h>(tnow);
    //potential_update_by_random_walk_core<side_v>();

    //cout << "- <End> Random Walk\n" << endl;
}

void core::random_walk_annealing_schedule(double tnow, double tend) {
    param.random_walk_step = 0.005+ (param.random_walk_step-0.005) * exp(-tnow / 2);
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