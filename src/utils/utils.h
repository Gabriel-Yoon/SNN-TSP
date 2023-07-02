#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
#include "py.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100
extern int INIT_VAL = 0;

using namespace std;

//**************************************************************************************************************//
namespace utils {

    void printProgress(double start, double end) {
        double percentage = start / end;
        int val = (int)(percentage * 100);
        if (val != INIT_VAL) {
            INIT_VAL++;
            int lpad = (int)(percentage * PBWIDTH);
            int rpad = PBWIDTH - lpad;
            printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
            fflush(stdout);
        }
    }

};

//**************************************************************************************************************//

#endif