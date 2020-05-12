#include "spike_control.hpp"
#include "core.hpp"

pair<double, boolean> prosign;

void core::assemble_prosign(){



}


void core::ext_spike_load() {

    pair<double, boolean> external_spike_list;
    
    int start_city = 5;

    int count = 0;
    int side;
    int neuron;
    double time;

    //priority_queue<sm_spk_one, vector<sm_spk_one>, sm_spk_one>queue_ext_pri;
    priority_queue<pair<double, sm_spk_one*>, vector<pair<double, sm_spk_one*>>, greater<pair<double, sm_spk_one*>>> queue_ext_pri;

    while(1) {

        sm_spk_one *spk = new sm_spk_one;
        spk->time = time;
        if(side == 0)
            spk->side = side_v;
        else
            spk->side = side_h;
        spk->neuron = neuron;
        queue_ext_pri.push(make_pair(time, spk));
        count++;
    }

    while(!queue_ext_pri.empty()) {
        sm_spk_one *spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        // Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
        if(phase->query_phase_d_or_m(ref_end_time) == sm_model_phase) {
            sm_spk *spk_ref = new sm_spk;
            spk_ref->time = ref_end_time;
            queue_ext.push(make_pair(ref_end_time, spk_ref));
        }

        sm_spk *spk_ext = new sm_spk; // Actual spike event
        spk_ext->spk.push_back(make_pair(spk_one->side, spk_one->neuron));

        //Find spikes at the same timing
        while(!queue_ext_pri.empty()) {
            sm_spk_one *spk_next = queue_ext_pri.top().second;
            if(spk_one->time != spk_next->time) { break; }
            spk_ext->spk.push_back(make_pair(spk_next->side, spk_next->neuron));
            delete spk_next;
            queue_ext_pri.pop();
        }

        // Create last_spk_st update event for ST_PAUSE
        sm_spk *spk_st_update = new sm_spk(*spk_ext);
        spk_st_update->time = spk_one->time;
        spk_st_update->st = true;
        queue_ext.push(make_pair(spk_st_update->time, spk_st_update));

        double time_fire2 = spk_one->time + param.delay_spikein2out;
        int phase_now = phase->query_phase_d_or_m(spk_one->time);

        // Create reset event
        if(param.hw_RES_EN) {
            sm_spk *spk_ext_reset = new sm_spk(*spk_ext);
            spk_ext_reset->time = time_fire2;
            spk_ext_reset->reset = true;
            queue_ext.push(make_pair(time_fire2, spk_ext_reset));
        }

        // Push spike event to queue
        if(!param.enable_gpgm) {
            spk_ext->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        } else {
            if(phase_now == sm_data_phase) {
                spk_ext->time = time_fire2 + param.delay_spikeout2wlr_data + param.wlr_width;
            } else {
                spk_ext->time = time_fire2 + param.delay_spikeout2wlr_model + param.wlr_width;
            }
        }
        queue_ext.push(make_pair(spk_ext->time, spk_ext));

        delete spk_one;
    }
}