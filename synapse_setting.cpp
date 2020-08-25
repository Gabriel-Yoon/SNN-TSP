#include <iostream>
#include <string>

#include "synapse.hpp"
#include "core.hpp"
#include "simulation_parameters.hpp"

using namespace std;

void core::weight_setup(){
	
    // Initializing max element as INT_MIN 
    double max_distance = 0;

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

    cout << "[START] WEIGHT_SETUP" << endl;
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
                    weight_matrix[v_idx][h_idx].Gp = 0.1 + (1- distance/max_distance)*0.1;
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


            // Check for weight value if they touch the boundaries
            double weight = weight_matrix[v_idx][h_idx].Gp;
            if (weight > max_w) {
                weight = max_w;
            }
            else if (weight < min_w) {
                weight = min_w;
            }

            //cout << weight_flag;
        }
        //printf("\n");
    }
    //printf("\n");
    cout << "[END] WEIGHT_SETUP" << endl;
}

inline void core::weight_set_gp(int v_idx, int h_idx) {
        double &weight = weight_matrix[v_idx][h_idx].Gp;
        double wt_delta_g = wt_delta_g_set[v_idx][h_idx].Gp;
		double max_w = 10;
        double min_w = 0;
        // double max_w = max_weight[v_idx][h_idx].Gp;
        // double min_w = min_weight[v_idx][h_idx].Gp;

        

        if(weight > max_w) {
                weight = max_w;
        } else if(weight < min_w) {
                weight = min_w;
        }
}
/*
inline void core::weight_set_gm(int v_idx, int h_idx) {
        double &weight = weight_matrix[v_idx][h_idx].Gm;
        double wt_delta_g = wt_delta_g_set[v_idx][h_idx].Gm;
        double max_w = 10;
        double min_w = 0;


  
        if(weight > max_w) {
                weight = max_w;
        } else if(weight < min_w) {
                weight = min_w;
        }
}

inline void core::weight_reset_gp(int v_idx, int h_idx) {
        if(rng_wt_reset_rate) {
                if(param.wt_reset_rate < rng_wt_reset_rate->get_val()) {
                        return;
                }
        }

        double &weight = weight_matrix[v_idx][h_idx].Gp;
        double wt_delta_g = wt_delta_g_reset[v_idx][h_idx].Gp;
        double max_w = max_weight[v_idx][h_idx].Gp;
        double min_w = min_weight[v_idx][h_idx].Gp;

        if(rng_wt_reset) {
                double &ideal = weight_matrix_ideal[v_idx][h_idx].Gp;
                ideal += wt_delta_g;
                if(weight < min_w) ideal = min_w;
                weight = ideal + rng_wt_reset->get_val();
        } else {
                weight += wt_delta_g;
        }

        if(weight > max_w) {
                weight = max_w;
        } else if(weight < min_w) {
                weight = min_w;
        }
}

inline void core::weight_reset_gm(int v_idx, int h_idx) {
        if(rng_wt_reset_rate) {
                if(param.wt_reset_rate < rng_wt_reset_rate->get_val()) {
                        return;
                }
        }

        double &weight = weight_matrix[v_idx][h_idx].Gm;
        double wt_delta_g = wt_delta_g_reset[v_idx][h_idx].Gm;
        double max_w = max_weight[v_idx][h_idx].Gm;
        double min_w = min_weight[v_idx][h_idx].Gm;

        if(rng_wt_reset) {
                double &ideal = weight_matrix_ideal[v_idx][h_idx].Gm;
                ideal += wt_delta_g;
                if(weight < min_w) ideal = min_w;
                weight = ideal + rng_wt_reset->get_val();
        } else {
                weight += wt_delta_g;
        }

        if(weight > max_w) {
                weight = max_w;
        } else if(weight < min_w) {
                weight = min_w;
        }
}

*/
