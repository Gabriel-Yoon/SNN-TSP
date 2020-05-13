#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

#include "simulation_parameters.hpp"

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

    simulation_parameters param;
    param.print_parameters();

    priority_queue<custom, vector<custom>, cmp> pq;
    pq.push(custom(5));
    pq.push(custom(2));
    pq.push(custom(8));
    pq.push(custom(9));
    pq.push(custom(1));
    pq.push(custom(14));

    while (!pq.empty()) {
        cout << pq.top().value << '\n';
        pq.pop();
    }

    return 0;
}
