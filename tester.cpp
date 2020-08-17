#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>  
#include <windows.h> // Sleep ÇÔ¼ö

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

int main(void) {
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

    return 0;
}