
#ifndef __PHASE_H__
#define __PHASE_H__

#include "param.h"

enum : int {
    data_phase,
    model_phase,
    transition_phase
};

class phase {

private:
    double total_time_per_pattern;
    double transition_time;
    double data_phase_time;
    double model_phase_time;

public:
    phase(param &params) {
        int transition = params.steps_transition;
        int data_phase = params.steps_data;
        int model_phase = params.steps_model;
        double scale = params.timestep;
        total_time_per_pattern = (double)(transition * 2 + data_phase + model_phase) * scale;
        transition_time = (double)transition * scale;
        data_phase_time = (double)data_phase * scale;
        model_phase_time = (double)model_phase * scale;
    }

    int query_phase(double time) {
        double stime = time - (double)((int)(time / total_time_per_pattern)) * total_time_per_pattern;
        int phase;

        if(((transition_time + FLOAT_EPSILON_TIME) < stime) &&
           (stime < (transition_time + data_phase_time - FLOAT_EPSILON_TIME)))
           phase = data_phase;
        else if(((transition_time * 2 + data_phase_time + FLOAT_EPSILON_TIME) < stime) &&
           (stime < (transition_time * 2 + data_phase_time + model_phase_time - FLOAT_EPSILON_TIME)))
           phase = model_phase;
        else
            phase = transition_phase;
        
        return phase;
    }

    int query_phase_d_or_m(double time) {
        double stime = time - (double)((int)(time / total_time_per_pattern)) * total_time_per_pattern;
        int phase;

        if((0.0 <= stime) && (stime < (transition_time + data_phase_time)))
            phase = data_phase;
        else
            phase = model_phase;
        
        return phase;
    }

    double get_time_per_image() {
        return transition_time * 2 + data_phase_time + model_phase_time;
    }

};

#endif // __PHASE_H__