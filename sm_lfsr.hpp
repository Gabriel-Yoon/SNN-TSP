
#ifndef __SM_LFSR_H__
#define __SM_LFSR_H__

#include <stdint.h>
#include <stdio.h>

// Galois Linear Feedback Shift Register

#define pval_32 0x80000057
#define pval_16 0x8016
#define pval_8  0x8E
#define pval_4  0x9

template <typename T> class Glfsr {
    private:
    T lfsr;

    protected:
    T polynomial;

    public:
    Glfsr() {}

    Glfsr(T start) {
        if(start == 0) printf("Error: start_state is 0.\n");
        lfsr = start;
    }

    T step() {
        T lsb = lfsr & 1;
        lfsr >>= 1;
        lfsr ^= (-lsb) & polynomial;
        return lfsr;
    }
};

class Glfsr_32: public Glfsr<uint32_t> {
    public:
        Glfsr_32(uint32_t start): Glfsr<uint32_t>(start) {
            polynomial = pval_32;
        }
};

class Glfsr_16: public Glfsr<uint16_t> {
    public:
        Glfsr_16(uint16_t start): Glfsr<uint16_t>(start) {
            polynomial = pval_16;
        }
};

class Glfsr_8: public Glfsr<uint8_t> {
    public:
        Glfsr_8(uint8_t start): Glfsr<uint8_t>(start) {
            polynomial = pval_8;
        }
};

/*
class Glfsr_4: public Glfsr<uint4_t> {
    public:
        Glfsr_4(uint4_t start): Glfsr<uint4_t>(start) {
            polynomial = pval_4;
        }
};
*/

#endif // __SM_LFSR_H__