#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  
//#include <windows.h> // Sleep �Լ�

#include "core.hpp"
#include "simulation_parameters.hpp"
#include "synapse.hpp"
#include "utils.hpp"
#include "tsp.hpp"

sm_spk* spk_null;

core::core()
{

}

void core::initialize()
{
    
    cout << "[START] CORE INITIALIZATION" << endl;

    weight_matrix.resize(num_neurons[side_v]);
    max_weight.resize(num_neurons[side_v]);
    min_weight.resize(num_neurons[side_v]);
    wt_delta_g_set.resize(num_neurons[side_v]);
    wt_delta_g_reset.resize(num_neurons[side_v]);
    for (int i = 0; i < num_neurons[side_v]; i++)
    {
        weight_matrix[i].resize(num_neurons[side_h]);
        max_weight[i].resize(num_neurons[side_h]);
        min_weight[i].resize(num_neurons[side_h]);
        wt_delta_g_set[i].resize(num_neurons[side_h]);
        wt_delta_g_reset[i].resize(num_neurons[side_h]);
    }

    // Neuron Info, will only be used in the hidden side of the neuron
    // because the visible side will only act as the external injection purpose.
    //potential[2] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_v], 32);
    //potential[2] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_h], 32);

    potential[side_v] = new double[num_neurons[side_v]];
    potential[side_h] = new double[num_neurons[side_h]];

    threshold[side_v] = new double[num_neurons[side_v]];
    threshold[side_h] = new double[num_neurons[side_h]];

    last_spk[side_v] = new double[num_neurons[side_v]];
    last_spk[side_h] = new double[num_neurons[side_h]];

    last_spk_st[side_v] = new double[num_neurons[side_v]];
    last_spk_st[side_h] = new double[num_neurons[side_h]];

    last_spk_in[side_v] = new double[num_neurons[side_v]];
    last_spk_in[side_h] = new double[num_neurons[side_h]];

    wsum[side_v] = new double[num_neurons[side_v]];
    wsum[side_h] = new double[num_neurons[side_h]];

    int i, j;
    for(i = 0; i < num_neurons[side_v]; i++) {
        potential[side_v][i] = 0.2;
        threshold[side_v][i] = 1;
        last_spk[side_v][i] = -1;
        last_spk_st[side_v][i] = -1;
        last_spk_in[side_v][i] = -1;
    }

    for(i = 0; i < num_neurons[side_h]; i++) {
        potential[side_h][i] = 0.2;
        threshold[side_h][i] = 1;
        last_spk[side_h][i] = -1;
        last_spk_st[side_h][i] = -1;
        last_spk_in[side_h][i] = -1;
    }


    weight_setup();

    cout << "[_END_] CORE INITIALIZATION\n" << endl;

}

int core::get_spk(sm_spk** spk_now, int* which_spk) {
    int num_spk_ext = queue_ext.size();
    int num_spk_int = queue_spk.size();

    if (num_spk_ext == 0) {
        if (num_spk_int == 0) { // No More event
            *spk_now = spk_null;
            return -1;
        }
        *spk_now = queue_spk.top().second; // internal event only
        *which_spk = spk_type_int;
        return 0;
    } else if (num_spk_int == 0) { // external event only
        *spk_now = queue_ext.top().second;
        *which_spk = spk_type_ext;
        return 0;
    }

    // When there are both external and interal events
    sm_spk* spk_ext = queue_ext.top().second;
    sm_spk* spk_int = queue_spk.top().second;

    if (fabs(spk_ext->time - spk_int->time) < FLOAT_EPSILON_TIME) {
        if ((spk_ext->reset == spk_int->reset) && (spk_ext->st == spk_int->st)) {
            spk_ext->merge(*spk_int);
            *spk_now = spk_ext;
            *which_spk = spk_type_both;
        } else {                    // Same timing but handle external spike first
            *spk_now = spk_ext;
            *which_spk = spk_type_ext;
        }
    } else if (spk_ext->time < spk_int->time) {         // Handle external spike
        *spk_now = spk_ext;
        *which_spk = spk_type_ext;
    } else {                                            // Handle internal spike
        *spk_now = spk_int;
        *which_spk = spk_type_int;
    }

    for (auto it = (*spk_now)->spk.begin(); it != (*spk_now)->spk.end(); it++) {

        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        //printf("Spike captured at WTA[%d][%d]\n", h_WTA, h_city);
    }
    return 0;
}

void core::ext_spike_load(double tend) {

    cout << "[START] EXTERNAL SPIKE LOAD" << endl;

    int neuron = 0; // start city : if city 1 is the start, neuron =0;
    double time;
    double injection_step = tend / param.timestep_injection;
    
    priority_queue<pair<double, sm_spk_one*>, vector<pair<double, sm_spk_one*>>, greater<pair<double, sm_spk_one*>>> queue_ext_pri;

    for (int i = 0; i < injection_step; i++) {
        sm_spk_one* spk = new sm_spk_one;
        spk->time = i * param.timestep_injection;
        spk->side = side_v;
        spk->neuron = neuron;
        queue_ext_pri.push(make_pair(spk->time, spk));
    }

    
    while (!queue_ext_pri.empty()) {
        sm_spk_one* spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        // Formulate Spike Template
        sm_spk* spk_ext = new sm_spk;
        spk_ext->spk.push_back(make_pair(spk_one->side, spk_one->neuron));

        double time_fire2 = spk_one->time + param.delay_spikein2out;

        // 1. Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
        sm_spk* spk_ref = new sm_spk(*spk_ext);
        spk_ref->time = ref_end_time;
        if (param.hw_CAP_ISO) {
            spk_ref->iso = -1;
        }
        queue_ext.push(make_pair(ref_end_time, spk_ref));

        spk_ext->iso = 0;
        // 2. Create last_spk_st update event for ST_PAUSE
        sm_spk* spk_st_update = new sm_spk(*spk_ext);
        spk_st_update->time = spk_one->time;
        spk_st_update->st = true;
        queue_ext.push(make_pair(spk_st_update->time, spk_st_update));

        // 3. Create reset event
        if (param.hw_RES_EN) {
            sm_spk* spk_ext_reset = new sm_spk(*spk_ext);
            spk_ext_reset->time = time_fire2;
            spk_ext_reset->reset = true;
            queue_ext.push(make_pair(time_fire2, spk_ext_reset));
        }

        // Push spike event to queue
        spk_ext->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        if (param.hw_CAP_ISO) {
            spk_ext->iso = 1;
        }
        queue_ext.push(make_pair(spk_ext->time, spk_ext));

        delete spk_one;
    }
    cout << "[_END_] EXTERNAL SPIKE LOAD\n" << endl;

}

template<int is_spk, int is_rng> void core::run_loop(double tnow, double tpre, double tend, sm_spk& spk_now, int which_spk, double& simtick, int& new_spk) {

    if (is_spk) {
        if(param.enable_learning)
            // fill it in!
    }

    potential_update_by_leak(tnow - tpre);

    if (is_spk) {
        potential_update_by_spk(spk_now);
    }

    if (is_rng) {
        if (param.enable_random_walk) {
            potential_update_by_random_walk(tnow);
            if (param.enable_simulated_annealing) {
                random_walk_annealing_schedule(tnow, tend);
                // printf("%f\n", param.random_walk_step);
            }
        }
        simtick += param.timestep_rng;
    }

    new_spk = compare_threshold(tnow, which_spk);
}

double core::run() {

    /* ------------------------------------------Simulation settings------------------------------------------ */
    double tend = 0.1;
    double tnow = 0.0;
    double tpre = 0.0;
    double simtick = param.timestep_rng;

    sm_spk* spk_now;
    int which_spk = 0;
    int is_spk = 0;
    int new_spk = 0;

    long int loop_count = 0;

    ext_spike_load(tend); //get queue_ext ready
    get_spk(&spk_now, &which_spk);
    
    //exportFile << spk_now->time << "," << spk_now->spk.begin()->first << "," << spk_now->spk.end()->second << "\n";
    // export_to_csv(exportFile, *spk_now, tend);
    
    // Console progress bar
    double consolepercentage;

    // ------------------------------------------------------------------------------------LOOP------------------------------------------------------------------------------------ START //

    while (tnow<=tend) {

        consolepercentage = tnow / tend;
        printProgress(consolepercentage);

        //cout << "------------------------------------------LOOP------------------------------------------" << endl;
        //cout << "tnow : " << tnow << endl;
        
        // get spike event
        if (is_spk) {
            if (which_spk == spk_type_ext) {
                delete queue_ext.top().second;
                queue_ext.pop();
            }
            else if (which_spk == spk_type_int) {
                delete queue_spk.top().second;
                queue_spk.pop();
            }
            else if (which_spk == spk_type_both) {
                delete queue_ext.top().second;
                queue_ext.pop();
                delete queue_spk.top().second;
                queue_spk.pop();
            }
        }

        if (new_spk || is_spk) {
            new_spk = 0;
            is_spk = 0;
            get_spk(&spk_now, &which_spk);
        }

        //cout << "simtick " << simtick << " spk time " << spk_now->time << endl;

        if (fabs(simtick - spk_now->time) < FLOAT_EPSILON_TIME) { // simtick == spk_now.time
            tnow = simtick;
            if (tnow > tend) break;
            is_spk = 1;
            if (spk_now->reset == true) {
                potential_reset(*spk_now); // Reset before or after run_loop()???
                run_loop<0, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            } else if (spk_now->st == true) {
                last_spk_st_update(*spk_now);
                run_loop<0, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            } else {
                run_loop<1, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }
        else if (simtick < spk_now->time) {
            tnow = simtick;
            if (tnow > tend) break;
            is_spk = 0;
            run_loop<0, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
            tpre = tnow;
        }
        else { // if simtick > spk_now.time
            tnow = spk_now->time;
            if (tnow > tend) break;
            is_spk = 1;
            if (spk_now->reset == true) {
                potential_reset(*spk_now);
            } else if (spk_now->st == true) {
                last_spk_st_update(*spk_now);
            } else {
                run_loop<1, 0>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }
        loop_count++;
    }

    return tnow;

}