#include "spike_control.hpp"
#include "core.hpp"


/*
void core::assemble_prosign(sm_spk *spk_now, bool overlap, int new_spk)
{
	// overlap should be defined as the boolean which tells
	// tnow is the multiple of timestep_injection.

	// The role of 'assemble_prosign' is to check the new spike and assemble prosign
	// th

	// prosign acts as the counter scan chain
	// whenever the spike is generated in the hidden neuron layer,
	// the counter scan chain delivers the spike info to the prosign.
	// The prosign is then injected to the visible neuron layer to generate external spikes.

    // prosign : list<pair<num_neuron, spike_fire>>
    if(overlap && new_spk){
    	inject_prosign.push_pack(make_pair(spk_now ))
    }
	if(new_spk)

    inject_prosign.push_pack(make_pair());

/*
    // Refractory time ends = opens the gate for potential update!
    double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
    sm_spk *spk_ref = new sm_spk;
    spk_ref->time = ref_end_time;
    prosign.push(make_pair(ref_end_time, spk_ref));
*/  


