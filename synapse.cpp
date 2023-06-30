#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include "synapse.hpp"
#include "core.hpp"
#include "simulation_parameters.hpp"

using namespace std;

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
