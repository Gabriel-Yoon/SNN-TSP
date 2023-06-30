
#ifndef __SM_RNG_HPP__
#define __SM_RNG_HPP__

#include <random>

using namespace std;

class sm_rng_1bit {
    public:
        virtual int get_val() = 0;
};

#include "sm_lfsr.hpp"

class sm_rng_1bit_lfsr32 : public Glfsr_32, public sm_rng_1bit {
    public:
    sm_rng_1bit_lfsr32(uint32_t start): Glfsr_32(start) {};
    int get_val() {
        return (int)(step() & 0x1);
    }
};

class sm_rng_1bit_lfsr16 : public Glfsr_16, public sm_rng_1bit {
    public:
    sm_rng_1bit_lfsr16(uint16_t start): Glfsr_16(start) {};
    int get_val() {
        return (int)(step() & 0x1);
    }
};

class sm_rng_1bit_lfsr8 : public Glfsr_8, public sm_rng_1bit {
    public:
    sm_rng_1bit_lfsr8(uint8_t start): Glfsr_8(start) {};
    int get_val() {
        return (int)(step() & 0x1);
    }
};

class sm_rng_1bit_mt : public sm_rng_1bit {

    public:
    mt19937 engine;
    uniform_int_distribution<> rng;

    sm_rng_1bit_mt(int seed) {
        engine = mt19937(seed);
        rng = uniform_int_distribution<>(0, 1);
    }

    int get_val() {
        return rng(engine) & 0x1;
    }
};

class sm_rng_ureal01 {

    public:
    mt19937 engine;
    uniform_real_distribution<> rng;

    sm_rng_ureal01(int seed) {
        engine = mt19937(seed);
        rng = uniform_real_distribution<>(0.0, 1.0);
    }

    double get_val() {
        return rng(engine);
    }
};

class sm_rng_normal {

    public:
    mt19937 engine;
    normal_distribution<double> rng;

    sm_rng_normal(int seed, double mean, double sigma) {
        engine = mt19937(seed);
        rng = normal_distribution<double>(mean, sigma);
    }

    double get_val() {
        return rng(engine);
    }
};

#endif // __SM_RNG_HPP__