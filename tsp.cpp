
#include <sstream>
#include <fstream>

#include "tsp.hpp"
#include "smsim.hpp"

using namespace std;

extern double INIT_PERFORMANCE = 0;

tsp::tsp() {

    // Later change it to JSON parser if JSON exists, JSON parse. else use below code!
    
    // Parameters
	num_city = 26;
	answer = 937; // 19 for 5 cities, 937 for 26 cities.
	total_distance;
	performance; // later change it to (time,performance) format
    mode = bilateral_BM;

    // TSP Inhibition Weight Setup
    same_WTA_diff_cities = 0;
    adj_WTA_same_cities = 0;
    non_adj_WTA_same_cities = 0;
    b_value = 0.15;
    gradient = 0.1;

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
   neuron_setup();
   if (mode == lateral_BM){
    weight_setup_lateral_BM();
   } else if (mode == bilateral_BM){
    weight_setup_bilateral_BM();
   }
}

// Set number of neurons in v,h layer
void tsp::neuron_setup() {
    if (num_city % 2 == 0){ // even number of city
        num_neurons[side_v] = 
        num_neurons[side_h] = 
    } else { // odd number of city
        num_neurons[side_v] = num_city * 
        num_neurons[side_h] =   
    }
    /*
    core.num_neurons[side_v] = neurons_visible_city;
    core.num_neurons[side_h] = neurons_hidden_city;
    
    core.num_neurons_bias[0] = neurons_visible_bias;
    core.num_neurons_bias[1] = neurons_hidden_bias;
    */
}

void tsp::weight_setup_lateral_BM(){
	
    // Initializing max element as INT_MIN 
    double max_distance = 0;
    ofstream exportFile_weight_setup;
    exportFile_weight_setup.open("weight_setup.csv");
    // checking each element of matrix 
    // if it is greater than maxElement, 
    // update maxElement

    for (int i = 0; i < num_city; i++) {
        for (int j = 0; j < num_city; j++) {
            if (distance_matrix[i][j] > max_distance) {
                max_distance = distance_matrix[i][j];
            }
        }
    }

    cout << "[START] WEIGHT SETUP" << endl;
	int v_WTA, v_city, h_WTA, h_city;
	// i : WTA Network, j : City
    for (int v_idx = 0; v_idx < num_neurons[side_v]; v_idx++){
        for (int h_idx = 0; h_idx < num_neurons[side_h]; h_idx++){

			double max_w = 10;
        	double min_w = 0;
            v_WTA = v_idx / num_city + 1;
            v_city = v_idx % num_city + 1;
            h_WTA = h_idx / num_city + 1;
            h_city = h_idx % num_city + 1;

        	double distance = distance_matrix[v_city-1][h_city-1];
            std::string weight_flag;

            if(v_WTA == h_WTA){ // same WTA
            	if(v_city == h_city){ // same city
                    weight_matrix[v_idx][h_idx].Gp = 0;
                    weight_flag = "0";
            	}
            	else{ // in the same_WTA but btw different cities
                    weight_matrix[v_idx][h_idx].Gp = param.same_WTA_diff_cities;
                    weight_flag = "-";
            	}
            }
            else if(fabs((v_WTA )-(h_WTA )) == 1 || fabs((v_WTA)-(h_WTA)) == (num_city-1)){ // adjacent WTA
            	if(v_city == h_city){ // adjacent WTA - same city = inhibition!
                    weight_matrix[v_idx][h_idx].Gp = param.adj_WTA_same_cities;
                    weight_flag = "/";
            	}else{ // THE MOST IMPORTANT PART OF THE TRAVELING SALESMAN PROBLEM
                    weight_matrix[v_idx][h_idx].Gp = param.b_value + (1- distance/max_distance)*param.gradient;
                    weight_flag = "*";
            	}
            }
            else if(fabs((v_WTA % num_city) - (h_WTA % num_city)) != 1) { //non-adjacent WTA networks
            	if(v_city == h_city){
                    weight_matrix[v_idx][h_idx].Gp = param.non_adj_WTA_same_cities; // inhibition btw same cities
                    weight_flag = "|";
            	}else{ // no connection
                    weight_matrix[v_idx][h_idx].Gp = 0;
                    weight_flag = "0";
            	}
            }

            if (v_idx == 0 && h_city == 1) {
                weight_matrix[v_idx][h_idx].Gp = -0.3;
            }

            // Check for weight value if they touch the boundaries
            double weight = weight_matrix[v_idx][h_idx].Gp;
            if (weight > max_w) {
                weight = max_w;
            }
            else if (weight < min_w) {
                weight = min_w;
            }
            //cout << weight_flag;
            exportFile_weight_setup << weight_matrix[v_idx][h_idx].Gp << ",";
        }
        exportFile_weight_setup << "\n";
        //printf("\n");
    }
    //printf("\n");
    exportFile_weight_setup.close();
    cout << "[_END_] WEIGHT SETUP" << endl;
}

void tsp::wta_initialize(double* potential[], double init_val){
        // First WTA zeroing process
    for (int i = 0; i < num_city; i++) {
        potential[side_h][i] = init_val; // init_val should be param.pt_init
    }
}

void tsp::wta_condition_update(sm_spk& spk_now, double tnow, double tend) {

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

void tsp::cap_iso_update() { // This function works only when (param.hw_CAP_ISO) is true

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

void tsp::export_performance_info_to_csv(tsp& tsp, ofstream& exportFile, double tnow) {

    total_distance = 0;
    performance = 0;
    bool distance_defined = true;
    int arr[26+1] = {};
    int multiply_salesman_route = 1;

    for (int i = 1; i < num_city + 1; i++) {
        
        // Tour check
        int salesman_route = last_spike[i];
        if (arr[salesman_route] == 0) {
            arr[salesman_route] = 1;
        }
        else {
            distance_defined = false;
        }
        
        // Tour distance calculation
        int a, b;
        if (i == num_city) { // CASE 1: end city to city 1
            a = num_city;
            b = 1;
        }
        else { // CASE 2: normal case
            a = i + 1;
            b = i;
        }
        if (last_spike[a] != 0 && last_spike[b] != 0) {
            total_distance += distance_matrix[last_spike[a] - 1][last_spike[b] - 1];
        }
        else {
            distance_defined = false;
        }

        if (distance_defined == false) {
            break;
        }

    }

    for (int i = 1; i < num_city + 1; i++) {
        multiply_salesman_route = multiply_salesman_route * arr[i];
    }

    if (distance_defined || (multiply_salesman_route == 0)) {
        performance = (answer / total_distance);
        if (performance > 1) {
            performance = 0;
        }
    }
    else {
        performance = 0;
    }

    if (INIT_PERFORMANCE != performance) {
        exportFile << tnow << "," << performance << "\n";
        INIT_PERFORMANCE = performance;
    }

}

void tsp::export_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tnow, double tend) {

    // [FILE EXPORT] Export spike time and neuron index to csv file

    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;

        if (it->first == side_h && h_WTA == 2) {
            exportFile << spk_now.time << "," << h_city << "\n";
        }
    }
}

void tsp::export_wta_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

    // [FILE EXPORT] Export spike time and neuron index in specific wta module to csv file
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        exportFile << spk_now.time << "," << it->first << "," << it->second + 1 << "\n";
    }
}

void tsp::export_num_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

    // [FILE EXPORT] Export spike time and neuron index to csv file
    for (int i = 1; i < num_city + 1; i++) { // i : neuron_index
        exportFile << i << ",";
        for (int j = 1; j < num_city + 1; j++) { // j : WTA #
            exportFile << spike_counter[j][i].num_spike << ",";
        }
        exportFile << "\n";
    }
}

void tsp::export_potential_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

    // [FILE EXPORT] Export neuron potentials to csv file
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        exportFile << spk_now.time << ",";
        for (int i = 0; i < num_neurons[side_h]; i++) {
            exportFile << potential[side_h][i] << ",";
        }
        exportFile << "\n";
    }
}

/*
void core::export_last_spk_st_to_csv(ofstream& exportFile, sm_spk& spk_now, double tend) {
for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {
        int h_WTA = it->second / num_city + 1;
        int h_city = it->second % num_city + 1;
        exportFile << spk_now.time << ",";

        for (int i = 0; i < num_neurons[side_h]; i++) {
            exportFile << last_spk_st[side_h][i] << ",";
        }
*/
void tsp::export_travel_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend) {

    bool travel_flag = false;

    // [FILE EXPORT] Export travel sequence to csv file
    for (auto it = spk_now.spk.begin(); it != spk_now.spk.end(); it++) {

        exportFile << spk_now.time << ",";

        for (int i = 1; i < num_city + 1; i++) {
            for (int j = 1; j < num_city + 1; j++) {
                if (WTA[i][j].route == true) {
                    exportFile << j << ",";
                    travel_flag = true;
                }
            }

            // No city involved in the step 'i', then export 0 as default
            if (!travel_flag) {
                exportFile << 0 << ",";
            }

            travel_flag = false;
        }
        exportFile << "\n";
    }
}