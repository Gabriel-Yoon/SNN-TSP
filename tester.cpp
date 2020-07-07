#include <iostream>
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
}
