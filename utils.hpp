#include <vector>
#include <string>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 100
extern int INIT_VAL = 0;

using namespace std;

void printProgress(double percentage) {
    int val = (int)(percentage * 100);
    if (val != INIT_VAL) {
        INIT_VAL++;
        int lpad = (int)(percentage * PBWIDTH);
        int rpad = PBWIDTH - lpad;
        printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }
}