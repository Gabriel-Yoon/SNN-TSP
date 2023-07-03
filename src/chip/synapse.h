#ifndef _SYNAPSE_H_
#define _SYNAPSE_H_

#include <vector>
#include <string>
#include <cstdint>
#include <utility>

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
    /*---------------------method-----------------*/
    public: synapse() : Gp(0), Gm(0) {}


};
#endif