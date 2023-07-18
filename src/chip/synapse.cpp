#include "synapse.h"

//----------------------------------------------------
synapse::synapse(){
    Gp = 0.0;
    Gm = 0.0;

    maxGp = 10.0;
    minGp = -10.0;
    maxGm = 10.0;
    minGm = -10.0;
    
    dG_set = 0.01;
    dG_set_sigma_cycle = 0.0;
    dG_set_seed_cycle = 3;
    dG_set_sigma_device = 0.0;
    dG_set_seed_device = 4;

    dG_reset = -0.01;
    dG_reset_sigma_cycle = 0.0;
    dG_reset_seed_cycle = 5;
    dG_reset_sigma_device = 0.0;
    dG_reset_seed_device = 6;
}

//----------------------------------------------------
void synapse::setGp(){
    if(this->Gp + this->dG_set > this->maxGp){
        this->Gp = this->maxGp;
    } else{
        this->Gp += dG_set;
    }
}
//----------------------------------------------------
void synapse::resetGp(){
    if(this->Gp + this->dG_reset > this->minGp){
        this->Gp = this->minGp;
    } else{
        this->Gp += dG_reset;
    }
}
//----------------------------------------------------
void synapse::setGm(){
    if(this->Gm + this->dG_set > this->maxGm){
        this->Gm = this->maxGm;
    } else{
        this->Gm += dG_set;
    }
}
//----------------------------------------------------
void synapse::resetGm(){
    if(this->Gm + this->dG_reset < this->minGm){
        this->Gm = this->minGp;
    } else{
        this->Gm += dG_reset;
    }
}