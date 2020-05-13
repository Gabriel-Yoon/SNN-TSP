#include "spike_control.hpp"
#include "core.hpp"

void core::assemble_prosign()
{
    //prosign : list<pair<num_neuron, spike_fire>>
    inject_prosign.push_pack(make_pair<>);

/*
    // Refractory time ends = opens the gate for potential update!
    double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
    sm_spk *spk_ref = new sm_spk;
    spk_ref->time = ref_end_time;
    prosign.push(make_pair(ref_end_time, spk_ref));
*/  



}