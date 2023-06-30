#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  
//#include <windows.h> // Sleep �Լ�

#include "core.hpp"
#include "simulation_parameters.hpp"
#include "utils.hpp"

core::core()
{
    // Number of Neurons
    neurons_visible_city = num_city * num_city;
    neurons_hidden_city = num_city * num_city;

    neurons_visible_bias = 0;
    neurons_hidden_bias = 0;
    
    num_neurons[side_v] = neurons_visible_city;
    num_neurons[side_h] = neurons_hidden_city;
    
    num_neurons_bias[0] = neurons_visible_bias;
    num_neurons_bias[1] = neurons_hidden_bias;

}

void core::initialize()
{
    
    cout << "[START] CORE INITIALIZATION" << endl;
    
    // When you change the TSP cities, you should change int num_city in core.hpp

    WTA.resize((int)(num_city + 1));
    WTA_at_least_one_spike.resize((int)(num_city + 1));
    spike_counter.resize((int)(num_city + 1));
    last_spike.resize((int)(num_city + 1));

    for (int i = 0; i < num_city+1; i++)
    {
        WTA[i].resize((int)(num_city + 1));
        spike_counter[i].resize((int)(num_city + 1));
    }
    for (int i = 1; i < num_city + 1; i++) {
        WTA_at_least_one_spike[i] = { false };
        for (int j = 1; j < num_city + 1; j++) {
            WTA[i][j].route = false;
            WTA[i][j].iso = false;
            spike_counter[i][j].num_spike = 0;
        }
    }
    

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
    
    //cout << "-<Start> get_spk" << endl;
    
    int num_spk_ext = queue_ext.size();
    int num_spk_int = queue_spk.size();

    if (num_spk_ext == 0) {
        if (num_spk_int == 0) { // No More event
            //cout << "No more event " << "- (get_spk) CASE 1-1 " << endl;
            //*spk_now = spk_null;
            return -1;
        }                       // internal event only
        //cout << "Internal spike only " << "- (get_spk) CASE 1-2 " << endl;
        *spk_now = queue_spk.top().second;
        *which_spk = spk_type_int;
        return 0;
    }
    else if (num_spk_int == 0) { // external event only
        //cout << "External spike only " << "- (get_spk) CASE 1-3 " << endl;
        *spk_now = queue_ext.top().second;
        *which_spk = spk_type_ext;
        return 0;
    }

    //cout << "Both external and internal spikes exists inside the queue " << "- (get_spk) CASE 2 " << endl;

    // When there are both external and interal events!
    sm_spk* spk_ext = queue_ext.top().second;
    sm_spk* spk_int = queue_spk.top().second;

    if (fabs(spk_ext->time - spk_int->time) < FLOAT_EPSILON_TIME) {
        if ((spk_ext->reset == spk_int->reset) && (spk_ext->st == spk_int->st) && (spk_ext->iso == spk_int->iso)) {
            //cout << "- (get_spk) CASE 2-1 : Handle both spikes" << endl;
            spk_ext->merge(*spk_int);
            *spk_now = spk_ext;
            *which_spk = spk_type_both;
        }
        else {
            //cout << "- (get_spk) CASE 2-2 : Same timing but handle external spike first" << endl;
            *spk_now = spk_ext;
            *which_spk = spk_type_ext;
        }
    }
    else if (spk_ext->time < spk_int->time) {
        //cout << "- (get_spk) CASE 2-3 : Handle external spike" << endl;
        *spk_now = spk_ext;
        *which_spk = spk_type_ext;
    }
    else { // spk_ext->time > spk_int->time
        //cout << "- (get_spk) CASE 2-4 : Handle internal spike" << endl;
        *spk_now = spk_int;
        *which_spk = spk_type_int;
    }

    for (auto it = (*spk_now)->spk.begin(); it != (*spk_now)->spk.end(); it++) {

        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        //printf("Spike captured at WTA[%d][%d]\n", h_WTA, h_city);
    }

    //cout << "-<End> get_spk\n" << endl;

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

    // Leak is now unabled -> Leak is now abled (2020.09.06)
    potential_update_by_leak(tnow - tpre);

    if (is_spk) {
        wta_condition_update(spk_now, tnow, tend);
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
    double tend = 1;
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
    
    ///* [FILE EXPORT] Setting the export file *///
    ofstream exportFile_potential;
    ofstream exportFile_spike;
    ofstream exportFile_num_spike;
    ofstream exportFile_travel;
    ofstream exportFile_performance;

    string filename;
    string str1 = std::to_string(num_city);
    string str2 = std::to_string(param.same_WTA_diff_cities);
    string str3 = std::to_string(param.adj_WTA_same_cities);
    string str4 = std::to_string(param.non_adj_WTA_same_cities);

    string str7 = std::to_string(tend);

    //filename.append("TSP_");
    //filename.append("num_city=");
    //filename.append(str1);
    /*
    if (!param.hw_CAP_ISO) {
        filename.append(" ");
        filename.append("same_WTA_diff_cities= ");
        filename.append(str2);

        filename.append(" ");
        filename.append("adj_WTA_same_cities= ");
        filename.append(str3);

        filename.append(" ");
        filename.append("non_adj_WTA_same_cities= ");
        filename.append(str4);
    }
    else {
        filename.append("_");
        filename.append("CAP_ISO_ON");
    }
    */
    
    //filename.append(" ");
    filename.append("tend=");
    filename.append(str7);
    if (param.enable_simulated_annealing) {
        filename.append("_sa_on");
    }
    filename.append(".csv");

    /*
    // [FILE EXPORT] POTENTIAL FILE
    string filename_potential; // Make new filemane for potential csv file
    filename_potential.append("Potential_");
    filename_potential.append(filename);

    exportFile_potential.open(filename_potential);
    exportFile_potential << "time" << ",";
    for (int i = 1; i <= num_neurons[side_h]; i++) {
        exportFile_potential << "neuron_" << i << ",";
    }
    exportFile_potential << "\n";

    
    // [FILE EXPORT] Export initial potential
    exportFile_potential << 0 << ",";
    for (int i = 0; i < num_neurons[side_h]; i++) {
        exportFile_potential << potential[side_h][i] << ",";
    }
    exportFile_potential << "\n";


    // [FILE EXPORT] SPIKE FILE
    string filename_spike;
    filename_spike.append("Spike_");
    filename_spike.append(filename);

    exportFile_spike.open(filename_spike);
    exportFile_spike << "time" << "," << "neuron_index" << "\n";
    */
    // [FILE EXPORT] TRAVEL ROUTE FILE
    string filename_travel;
    filename_travel.append("Travel_");
    filename_travel.append(filename);

    exportFile_travel.open(filename_travel);
    exportFile_travel << "time" << ",";
    for (int i = 1; i < num_city+1; i++) {
        exportFile_travel << "step_" << i << ",";
    }
    exportFile_travel << "\n";
    

    // [FILE EXPORT] NUM SPIKE FILE
    string filename_num_spike;
    filename_num_spike.append("NumSpike_");
    filename_num_spike.append(filename);

    exportFile_num_spike.open(filename_num_spike);
    exportFile_num_spike << "neuron_index" << ",";
    for (int i = 1; i < num_city + 1; i++) {
        exportFile_num_spike << "step_" << i << ",";
    }
    exportFile_num_spike << "\n";

    // [FILE EXPORT] PERFORMANCE FILE
    string filename_performance;
    filename_performance.append("Performance_");
    filename_performance.append(filename);

    exportFile_performance.open(filename_performance);
    exportFile_performance << "time" << "," << "performance" << "\n";

    //exportFile_wta_spike.open(filename_num_spike);

    ///* [FILE EXPORT] END *///

    // Console progress bar
    double consolepercentage;

    // ------------------------------------------------------------------------------------LOOP------------------------------------------------------------------------------------ START //

    while (tnow<=tend) {

        consolepercentage = tnow / tend;
        printProgress(consolepercentage);

        //cout << "------------------------------------------LOOP------------------------------------------" << endl;
        //cout << "tnow : " << tnow << endl;

        // First WTA zeroing process
        for (int i = 0; i < num_city; i++) {
            potential[side_h][i] = param.pt_init;
        }

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
                //cout << "CASE 1-1 ***************************************" << endl;
                potential_reset(*spk_now); // Reset before or after run_loop()???
                run_loop<0, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else if (spk_now->st == true) {
                //cout << "CASE 1-2" << endl;
                last_spk_st_update(*spk_now);
                run_loop<0, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else {
                //cout << "CASE 1-3" << endl;
                run_loop<1, 1>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                //export_potential_info_to_csv(exportFile_potential, *spk_now, tend);
                if (spk_now->iso == 1 && tnow > tend - 5 && tnow < tend) { //Deleted && tnow > tend-10 && tnow < tend
                    export_spike_info_to_csv(exportFile_spike, *spk_now, tnow, tend);
                    export_travel_info_to_csv(exportFile_travel, *spk_now, tend);
                    export_performance_info_to_csv(exportFile_performance, tnow);
                }
                tpre = tnow;
            }
        }
        else if (simtick < spk_now->time) {
            //cout << "CASE 2" << endl;
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
                //cout << "CASE 3-1" << endl;
                potential_reset(*spk_now);
            }
            else if (spk_now->st == true) {
                //cout << "CASE 3-2" << endl;
                last_spk_st_update(*spk_now);
            }
            else {
                //cout << "CASE 3-3" << endl;
                run_loop<1, 0>(tnow, tpre, tend, *spk_now, which_spk, simtick, new_spk);
                //export_potential_info_to_csv(exportFile_potential, *spk_now, tend);
                if (spk_now->iso == 1 && tnow > tend - 5 && tnow < tend) { // Real spike event. Deleted && tnow > tend-10 && tnow < tend 
                    export_spike_info_to_csv(exportFile_spike, *spk_now, tnow, tend);
                    export_travel_info_to_csv(exportFile_travel, *spk_now, tend);
                    export_performance_info_to_csv(exportFile_performance, tnow);
                }
                tpre = tnow;
            }
        }
        loop_count++;
    }

    export_num_spike_info_to_csv(exportFile_num_spike, *spk_now, tend);

    //exportFile_potential.close();
    //exportFile_spike.close();
    exportFile_num_spike.close();
    exportFile_travel.close();
    exportFile_performance.close();

    return tnow;

}