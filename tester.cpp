#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

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
    core mycore;
    mycore.initialize();
    double a = mycore.run();

    // Make three vectors, each of length 100 filled with 1s, 2s, and 3s
    vector<int> vec1(100, 1);
    vector<int> vec2(100, 2);
    
    // Wrap into a vector
    vector<pair<string, vector<int>>> vals = {{"Time", vec1}, {"Neuron Index", vec2}};

    return 0;
}