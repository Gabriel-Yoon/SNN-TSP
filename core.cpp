#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>  
//#include <windows.h> // Sleep ÇÔ¼ö

#include "core.hpp"
#include "simulation_parameters.hpp"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100

using namespace std;

void printProgress(double percentage) {
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}



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


    /*
    // FIVE is a set of 5 cities. The minimal tour has length 19.
    // Answer : 1 - 3 - 2 - 5 - 4
    distance_matrix = {
        {0, 3, 4, 2, 7},
        {3, 0, 4, 6, 3},
        {4, 4, 0, 5, 8},
        {2, 6, 5, 0, 6},
        {7, 3, 8, 6, 0},
    };
    */
    
    
    // FRI26 TSP dataset from "https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html"
    // Answer : 1-25-24-23-26-22-21-17-18-20-19-16-11-12-13-15-14-10-9-8-7-5-6-4-3-2-1 (937)
    distance_matrix = {
        {0,   83,  93, 129, 133, 139, 151, 169, 135, 114, 110,  98,  99,  95,  81, 152, 159, 181, 172, 185, 147, 157, 185, 220, 127, 181},
        {83,   0,  40,  53,  62,  64,  91, 116,  93,  84,  95,  98,  89,  68,  67, 127, 156, 175, 152, 165, 160, 180, 223, 268, 179, 197},
        {93,  40,   0,  42,  42,  49,  59,  81,  54,  44,  58,  64,  54,  31,  36,  86, 117, 135, 112, 125, 124, 147, 193, 241, 157, 161},
        {129,  53,  42,   0,  11,  11,  46,  72,  65,  70,  88, 100,  89,  66,  76, 102, 142, 156, 127, 139, 155, 180, 228, 278, 197, 190},
        {133,  62,  42,  11,   0,   9,  35,  61,  55,  62,  82,  95,  84,  62,  74,  93, 133, 146, 117, 128, 148, 173, 222, 272, 194, 182},
        {139,  64,  49,  11,   9,   0,  39,  65,  63,  71,  90, 103,  92,  71,  82, 100, 141, 153, 124, 135, 156, 181, 230, 280, 202, 190},
        {151,  91,  59,  46,  35,  39,   0,  26,  34,  52,  71,  88,  77,  63,  78,  66, 110, 119,  88,  98, 130, 156, 206, 257, 188, 160},
        {169, 116,  81,  72,  61,  65,  26,   0,  37,  59,  75,  92,  83,  76,  91,  54,  98, 103,  70,  78, 122, 148, 198, 250, 188, 148},
        {135,  93,  54,  65,  55,  63,  34,  37,   0,  22,  39,  56,  47,  40,  55,  37,  78,  91,  62,  74,  96, 122, 172, 223, 155, 128},
        {114,  84,  44,  70,  62,  71,  52,  59,  22,   0,  20,  36,  26,  20,  34,  43,  74,  91,  68,  82,  86, 111, 160, 210, 136, 121},
        {110,  95,  58,  88,  82,  90,  71,  75,  39,  20,   0,  18,  11,  27,  32,  42,  61,  80,  64,  77,  68,  92, 140, 190, 116, 103},
        {98,  98,  64, 100,  95, 103,  88,  92,  56,  36,  18,   0,  11,  34,  31,  56,  63,  85,  75,  87,  62,  83, 129, 178, 100,  99},
        {99,  89,  54,  89,  84,  92,  77,  83,  47,  26,  11,  11,   0,  23,  24,  53,  68,  89,  74,  87,  71,  93, 140, 189, 111, 107},
        {95,  68,  31,  66,  62,  71,  63,  76,  40,  20,  27,  34,  23,   0,  15,  62,  87, 106,  87, 100,  93, 116, 163, 212, 132, 130},
        {81,  67,  36,  76,  74,  82,  78,  91,  55,  34,  32,  31,  24,  15,   0,  73,  92, 112,  96, 109,  93, 113, 158, 205, 122, 130},
        {152, 127,  86, 102,  93, 100,  66,  54,  37,  43,  42,  56,  53,  62,  73,   0,  44,  54,  26,  39,  68,  94, 144, 196, 139,  95},
        {159, 156, 117, 142, 133, 141, 110,  98,  78,  74,  61,  63,  68,  87,  92,  44,   0,  22,  34,  38,  30,  53, 102, 154, 109,  51},
        {181, 175, 135, 156, 146, 153, 119, 103,  91,  91,  80,  85,  89, 106, 112,  54,  22,   0,  33,  29,  46,  64, 107, 157, 125,  51},
        {172, 152, 112, 127, 117, 124,  88,  70,  62,  68,  64,  75,  74,  87,  96,  26,  34,  33,   0,  13,  63,  87, 135, 186, 141,  81},
        {185, 165, 125, 139, 128, 135,  98,  78,  74,  82,  77,  87,  87, 100, 109,  39,  38,  29,  13,   0,  68,  90, 136, 186, 148,  79},
        {147, 160, 124, 155, 148, 156, 130, 122,  96,  86,  68,  62,  71,  93,  93,  68,  30,  46,  63,  68,   0,  26,  77, 128,  80,  37},
        {157, 180, 147, 180, 173, 181, 156, 148, 122, 111,  92,  83,  93, 116, 113,  94,  53,  64,  87,  90,  26,   0,  50, 102,  65,  27},
        {185, 223, 193, 228, 222, 230, 206, 198, 172, 160, 140, 129, 140, 163, 158, 144, 102, 107, 135, 136,  77,  50,   0,  51,  64,  58},
        {220, 268, 241, 278, 272, 280, 257, 250, 223, 210, 190, 178, 189, 212, 205, 196, 154, 157, 186, 186, 128, 102,  51,   0,  93, 107},
        {127, 179, 157, 197, 194, 202, 188, 188, 155, 136, 116, 100, 111, 132, 122, 139, 109, 125, 141, 148,  80,  65,  64,  93,   0,  90},
        {181, 197, 161, 190, 182, 190, 160, 148, 128, 121, 103,  99, 107, 130, 130,  95,  51,  51,  81,  79,  37,  27,  58, 107,  90,   0},
    };
    
    /*
    // GR17 TSP dataset from "https://people.sc.fsu.edu/~jburkardt/datasets/tsp/tsp.html"
    // Answer : 1-4-13-7-8-56-17-14-15-3-11-10-2-5-9-12-16 (2085)
    distance_matrix = {
        {  0, 633, 257,  91, 412, 150,  80, 134, 259, 505, 353, 324,  70, 211, 268, 246, 121},
        {633,   0, 390, 661, 227, 488, 572, 530, 555, 289, 282, 638, 567, 466, 420, 745, 518},
        {257, 390,   0, 228, 169, 112, 196, 154, 372, 262, 110, 437, 191,  74,  53, 472, 142},
        { 91, 661, 228,   0, 383, 120,  77, 105, 175, 476, 324, 240,  27, 182, 239, 237,  84},
        {412, 227, 169, 383,   0, 267, 351, 309, 338, 196,  61, 421, 346, 243, 199, 528, 297},
        {150, 488, 112, 120, 267,   0,  63,  34, 264, 360, 208, 329,  83, 105, 123, 364,  35},
        { 80, 572, 196,  77, 351,  63,   0,  29, 232, 444, 292, 297,  47, 150, 207, 332,  29},
        {134, 530, 154, 105, 309,  34,  29,   0, 249, 402, 250, 314,  68, 108, 165, 349,  36},
        {259, 555, 372, 175, 338, 264, 232, 249,   0, 495, 352,  95, 189, 326, 383, 202, 236},
        {505, 289, 262, 476, 196, 360, 444, 402, 495,   0, 154, 578, 439, 336, 240, 685, 390},
        {353, 282, 110, 324,  61, 208, 292, 250, 352, 154,   0, 435, 287, 184, 140, 542, 238},
        {324, 638, 437, 240, 421, 329, 297, 314,  95, 578, 435,   0, 254, 391, 448, 157, 301},
        { 70, 567, 191,  27, 346,  83,  47,  68, 189, 439, 287, 254,   0, 145, 202, 289,  55},
        {211, 466,  74, 182, 243, 105, 150, 108, 326, 336, 184, 391, 145,   0,  57, 426,  96},
        {268, 420,  53, 239, 199, 123, 207, 165, 383, 240, 140, 448, 202,  57,   0, 483, 153},
        {246, 745, 472, 237, 528, 364, 332, 349, 202, 685, 542, 157, 289, 426, 483,   0, 336},
        {121, 518, 142, 84 ,297 , 35 , 29 , 36 , 236, 390, 238, 301,  55,  96, 153, 336,   0},
    };
    */


    /*
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

    */

    WTA.resize((int)(num_city + 1));
    spike_counter.resize((int)(num_city + 1));
    last_spike.resize((int)(num_city + 1));

    for (int i = 0; i < num_city+1; i++)
    {
        WTA[i].resize((int)(num_city + 1));
        spike_counter[i].resize((int)(num_city + 1));
    }
    for (int i = 1; i < num_city + 1; i++) {
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
            }
        }
        simtick += param.timestep_rng;
    }

    new_spk = compare_threshold(tnow);
}

double core::run() {

    /* ------------------------------------------Simulation settings------------------------------------------ */
    double tend = 10;
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
                if (spk_now->iso == 1 && tnow > tend - 1 && tnow < tend) { // Real spike event. Deleted && tnow > tend-10 && tnow < tend 
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