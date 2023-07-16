#ifndef _SYNAPSE_H_
#define _SYNAPSE_H_

#include <vector>
#include <string>
#include <cstdint>
#include <utility>

#include "param.h"
#include "rng.h"

enum : int {
    side_v,
    side_h
};

extern char side_chr[2];

// Synapse 특성을 새로 도입할 경우 바꿀 수 있는 인자들을 모아서 바꿔보자.

class synapse {

    friend class core;
    friend class spk;
    /*---------------------fields-----------------*/
    public: double Gp;
    public: double Gm;
    
    public: double maxGp;
    public: double minGp;
    public: double maxGm;
    public: double minGm;

    public: double dG_set;
    public: double dG_set_sigma_cycle;
    public: double dG_set_seed_cycle;
    public: double dG_set_sigma_device;
    public: double dG_set_seed_device;

    public: double dG_reset;
    public: double dG_reset_sigma_cycle;
    public: double dG_reset_seed_cycle;
    public: double dG_reset_sigma_device;
    public: double dG_reset_seed_device;

    /*---------------------method-----------------*/
    public: synapse();
    public: ~synapse(){};
    public: void setGp();
    public: void resetGp();
    public: void setGm();
    public: void resetGm();

};
#endif