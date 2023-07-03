// Control Potential of Neurons
#include <cassert>
#include <iostream>
#include <math.h>
#include <random>
#include <chrono>
#include <string.h>

#include "core.h"

template<int side> void core::potential_update_by_spk_core(spk& spk_now, double* wsum) {
    double last_spk_refractory = spk_now._spkTime - params.refractory_time;
    if (last_spk_refractory < 0.0) last_spk_refractory = 0.0;

    for (int idx = 0; idx < num_neurons[side]; idx++) {
        int h_WTA = idx / _tsp->_numCity + 1;
        int h_city = idx % _tsp->_numCity + 1;

        //bool not_in_ref_st = layers[side][idx]->_lastSpkTimeST_PAUSE < last_spk_refractory;
        //bool not_in_ref_in = layers[side][idx]->_lastSpkTimeIN_PAUSE < last_spk_refractory;
        //bool not_in_cap_iso = !WTA[h_WTA][h_city].iso;
        // removed && not_in_cap_iso from the if below
        if (layers[side][idx]->memV() + wsum[idx] * params.pt_alpha <= 0) {
            layers[side][idx]->set_memV(0);
        }
        else {
            layers[side][idx]->_memV += wsum[idx] * params.pt_alpha;
        }
        /*
        if (not_in_ref_st && (!param.hw_ISO_MOD || not_in_ref_in) ) {

            if (wsum[idx] * param.pt_alpha != 0) {
                // printf("Potential of WTA[%d][%d] rose by %lf \n", h_WTA, h_city, wsum[idx] * param.pt_alpha);
            }

        }
        else {
            //cout << "EXCEPTION AT : " << idx + 1 << endl;
        }
        */

    }
}

void core::potential_update_by_spk(spk& spk_now) {

    //cout << "- <Start> Potential Update by Spike" << endl;

    // Clear out wsum
    memset(wsum[side_v], 0, sizeof(double) * num_neurons[side_v]);
    memset(wsum[side_h], 0, sizeof(double) * num_neurons[side_h]);

    int spk_side_v = 0;
    int spk_side_h = 0;

    // Caution : do not use continue option! This it stands for each spike infos.
    for (auto it = spk_now._spk.begin(); it != spk_now._spk.end(); it++) {

        int h_WTA = it->second / _tsp->_numCity + 1;
        int h_city = it->second % _tsp->_numCity + 1;

        if (it->first == side_v) { // External spike at city 1 (only)

            spk_side_v = 1;

            //printf("[EXT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);

            for (int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (synapses[it->second][i]->Gp - synapses[it->second][i]->Gm);
            }

        }
        else { // side_h
         // This stands for internal spikes!
            // SYNCHRONIZATION : spike side is changed from side_h to side_v

            spk_side_v = 1;

            //printf("[INT] Spike is generating at WTA[%d][%d]\n", h_WTA, h_city);

            for (int i = 0; i < num_neurons[side_h]; i++) {
                wsum[side_h][i] += (synapses[it->second][i]->Gp - synapses[it->second][i]->Gm);
            }

        }

    }

    if (spk_side_h) potential_update_by_spk_core<side_v>(spk_now, wsum[side_v]);
    if (spk_side_v) potential_update_by_spk_core<side_h>(spk_now, wsum[side_h]);

    //cout << "- <End> Potential Update by Spike\n" << endl;

}
