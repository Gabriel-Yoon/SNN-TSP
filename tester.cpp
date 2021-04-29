#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

//#include <windows.h> // Sleep �Լ�

#include "core.hpp"

using namespace std;

struct custom {
    int x, y, value;
    custom(int v) : x(0), y(0), value(v) {}
};

struct cmp {
    bool operator()(const custom& a, const custom& b) {
        return a.value > b.value;
    }
};

int main(int argc, char *argv[]) {
    clock_t start, end;
    double result;

    start = clock();
    
    system("color 0A");
    core mycore;
    mycore.initialize();
    
    
    double a = mycore.run();

    /*
    // TSP Inhibition Weight Setup
    double same_WTA_diff_cities = -0.1;
    double adj_WTA_same_cities = -0.1;
    double non_adj_WTA_same_cities = -0.1;

    mycore.weight_setup(same_WTA_diff_cities, adj_WTA_same_cities, non_adj_WTA_same_cities);
    */

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    cout << "User time " << usage.ru_utime.tv_sec + usage.ru_utime.tv_usec * 10e-7 << "sec" << endl;
    cout << "System time" << usage.ru_stime.tv_sec + usage.ru_utime.tv_usec * 10e-7 << "sec" << endl;
    cout << "maxurss " << usage.ru_maxrss / 1024 << " MB" << endl;

    // Time print
    end = clock();
    result = (double)(end - start);
    cout << "result : " << ((result) / CLOCKS_PER_SEC) << " seconds" << endl;
    printf("%f", result / CLOCKS_PER_SEC);


}