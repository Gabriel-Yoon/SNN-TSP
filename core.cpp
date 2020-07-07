
#include <iostream>

#include "core.hpp"
#include "simulation_parameters.hpp"

using namespace std;

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

    cout << "[START] CORE_INITIALIZATION" << endl;
    
    // TSP dataset from "https://developers.google.com/optimization/routing/tsp#c++"
    distance_matrix = {
        {0, 2451, 713, 1018, 1631, 1374, 2408, 213, 2571, 875, 1420, 2145, 1972},
        {2451, 0, 1745, 1524, 831, 1240, 959, 2596, 403, 1589, 1374, 357, 579},
        {713, 1745, 0, 355, 920, 803, 1737, 851, 1858, 262, 940, 1453, 1260},
        {1018, 1524, 355, 0, 700, 862, 1395, 1123, 1584, 466, 1056, 1280, 987},
        {1631, 831, 920, 700, 0, 663, 1021, 1769, 949, 796, 879, 586, 371},
        {1374, 1240, 803, 862, 663, 0, 1681, 1551, 1765, 547, 225, 887, 999},
        {2408, 959, 1737, 1395, 1021, 1681, 0, 2493, 678, 1724, 1891, 1114, 701},
        {213, 2596, 851, 1123, 1769, 1551, 2493, 0, 2699, 1038, 1605, 2300, 2099},
        {2571, 403, 1858, 1584, 949, 1765, 678, 2699, 0, 1744, 1645, 653, 600},
        {875, 1589, 262, 466, 796, 547, 1724, 1038, 1744, 0, 679, 1272, 1162},
        {1420, 1374, 940, 1056, 879, 225, 1891, 1605, 1645, 679, 0, 1017, 1200},
        {2145, 357, 1453, 1280, 586, 887, 1114, 2300, 653, 1272, 1017, 0, 504},
        {1972, 579, 1260, 987, 371, 999, 701, 2099, 600, 1162, 1200, 504, 0},
    };
    
    for (int i = 0; i < num_city+1; i++) {
        for (int j = 0; j < num_city+1; j++) {
            WTA[i][j] = false;
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
    potential[2] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_v], 32);
    potential[2] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_h], 32);

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
        potential[side_v][i] = 0.0;
        threshold[side_v][i] = 1;
        last_spk[side_v][i] = -1;
        last_spk_st[side_v][i] = -1;
        last_spk_in[side_v][i] = -1;
    }

    for(i = 0; i < num_neurons[side_h]; i++) {
        potential[side_h][i] = 0.0;
        threshold[side_h][i] = 1;
        last_spk[side_h][i] = -1;
        last_spk_st[side_h][i] = -1;
        last_spk_in[side_h][i] = -1;
    }

    weight_setup();

    cout << "[END] CORE_INITIALIZATION\n" << endl;

}

int core::get_spk(sm_spk** spk_now, int* which_spk) {
    
    cout << "-<Start> get_spk" << endl;
    
    int num_spk_ext = queue_ext.size();
    int num_spk_int = queue_spk.size();

    if (num_spk_ext == 0) {
        if (num_spk_int == 0) { // No More event
            cout << "No more event " << "- (get_spk) CASE 1-1 " << endl;
            //*spk_now = spk_null;
            return -1;
        }                       // internal event only
        cout << "Internal spike only " << "- (get_spk) CASE 1-2 " << endl;
        *spk_now = queue_spk.top().second;
        *which_spk = spk_type_int;
        return 0;
    }
    else if (num_spk_int == 0) { // external event only
        cout << "External spike only " << "- (get_spk) CASE 1-3 " << endl;
        *spk_now = queue_ext.top().second;
        *which_spk = spk_type_ext;
        return 0;
    }

    cout << "Both external and internal spikes exists inside the queue " << "- (get_spk) CASE 2 " << endl;

    // When there are both external and interal events!
    sm_spk* spk_ext = queue_ext.top().second;
    sm_spk* spk_int = queue_spk.top().second;

    if (fabs(spk_ext->time - spk_int->time) < FLOAT_EPSILON_TIME) {
        if ((spk_ext->reset == spk_int->reset) && (spk_ext->st == spk_int->st)) {
            cout << "- (get_spk) CASE 2-1 : Handle both spikes" << endl;
            spk_ext->merge(*spk_int);
            *spk_now = spk_ext;
            *which_spk = spk_type_both;
        }
        else {
            cout << "- (get_spk) CASE 2-2 : Same timing but handle external spike first" << endl;
            *spk_now = spk_ext;
            *which_spk = spk_type_ext;
        }
    }
    else if (spk_ext->time < spk_int->time) {
        cout << "- (get_spk) CASE 2-3 : Handle external spike" << endl;
        *spk_now = spk_ext;
        *which_spk = spk_type_ext;
    }
    else { // spk_ext->time > spk_int->time
        cout << "- (get_spk) CASE 2-4 : Handle internal spike" << endl;
        *spk_now = spk_int;
        *which_spk = spk_type_int;
    }

    for (auto it = (*spk_now)->spk.begin(); it != (*spk_now)->spk.end(); it++) {

        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        printf("Spike captured at WTA[%d][%d]\n", h_WTA, h_city);
    }

    cout << "-<End> get_spk\n" << endl;

    return 0;
}

void core::ext_spike_load(double tend) {

    cout << "[START] EXTERNAL SPIKE_LOAD" << endl;

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


    // Only two events will be covered : Potential reset event and Actual spike event
    while (!queue_ext_pri.empty()) {
        sm_spk_one* spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        /*
        // Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
        sm_spk* spk_ref = new sm_spk;
        spk_ref->time = ref_end_time;
        queue_ext.push(make_pair(ref_end_time, spk_ref));

        // Create last_spk_st update event for ST_PAUSE
        sm_spk* spk_st_update = new sm_spk(*spk_ext);
        spk_st_update->time = spk_one->time;
        spk_st_update->st = true;
        queue_ext.push(make_pair(spk_st_update->time, spk_st_update));

        */

        // Formulate Spike Template
        sm_spk* spk_ext = new sm_spk;
        spk_ext->spk.push_back(make_pair(spk_one->side, spk_one->neuron));

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
    cout << "[END] EXTERNAL_SPIKE_LOAD" << endl;

}

template<int is_spk, int is_rng> void core::run_loop(double tnow, double tpre, sm_spk& spk_now, int which_spk, double& simtick, int& new_spk) {

    //potential_update_by_leak(tnow - tpre);

    if (is_spk) {
        potential_update_by_spk(spk_now);
    }
    
    if (is_rng) {
        if (param.enable_random_walk) {
            potential_update_by_random_walk();
        }
        simtick += param.timestep_rng;
    }
    
    new_spk = compare_threshold(tnow);

}

double core::run() {

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
    
    cout << spk_now->time << endl;
    
    while (1) {

        cout << "------------------------------------------LOOP------------------------------------------" << endl;
        cout << "tnow : " << tnow << endl;

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

        cout << "simtick " << simtick << " spk time " << spk_now->time << endl;

        if (fabs(simtick - spk_now->time) < FLOAT_EPSILON_TIME) { // simtick == spk_now.time
            tnow = simtick;
            if (tnow > tend) break;
            is_spk = 1;
            if (spk_now->reset == true) {
                cout << "CASE 1-1" << endl;
                potential_reset(*spk_now); // Reset before or after run_loop()???
                run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else if (spk_now->st == true) {
                cout << "CASE 1-2" << endl;
                last_spk_st_update(*spk_now);
                run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else {
                cout << "CASE 1-3" << endl;
                run_loop<1, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }
        else if (simtick < spk_now->time) {
            cout << "CASE 2" << endl;
            tnow = simtick;
            if (tnow > tend) break;
            is_spk = 0;
            run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
            tpre = tnow;
        }
        else { // if simtick > spk_now.time
            tnow = spk_now->time;
            if (tnow > tend) break;
            is_spk = 1;
            if (spk_now->reset == true) {
                cout << "CASE 3-1" << endl;
                potential_reset(*spk_now);
            }
            else if (spk_now->st == true) {
                cout << "CASE 3-2" << endl;
                last_spk_st_update(*spk_now);
            }
            else {
                cout << "CASE 3-3" << endl;
                run_loop<1, 0>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }
        
        // Traveling route check
        for (int i = 1; i < num_city+1; i++) {
            for (int j = 1; j < num_city+1; j++) {
                if (WTA[i][j]) {
                    printf("Travel city %d at step %d\n", j, i);
                }
            }
        }

        loop_count++;
    }
    
    return tnow;

}