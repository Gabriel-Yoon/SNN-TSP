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

    int num_city = 5;
    int neurons_visible_city = num_city * num_city;
    int neurons_hidden_city = num_city * num_city;
    
    int num_neurons[2];
    int WTA[num_city+1][num_city+1]; // indexing array

    num_neurons[side_v] = neurons_visible_city;
    num_neurons[side_h] = neurons_hidden_city;

    double *potential[num_city];
    double *threshold[num_city];
    double *last_spk[num_city];    // Used for weight_update
    double *last_spk_st[num_city]; // Used for SI_PAUSE in neuron
    double *last_spk_in[num_city];    
    
    potential[num_city] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_v], 32);
    potential[num_city] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_h], 32);

    potential[side_v] = new double[num_neurons[side_v]];
    potential[side_h] = new double[num_neurons[side_h]];

    threshold[side_v] = new double[num_neurons[side_v]];
    threshold[side_h] = new double[num_neurons[side_h]];

    last_spk[side_v] = new double[num_neurons[side_v]];
    last_spk[side_h] = new double[num_neurons[side_h]];

    last_spk_st[side_v] = new double[num_neurons[side_v]];
    last_spk_st[side_h] = new double[num_neurons[side_h]];

    last_spk_in[side_v] = new double[num_neurons[side_v]];
    last_spk_in[side_h] = new double[num_neurons[side_h]];

    for (int i = 1; i < num_city+1; i++){
        for (int j = 1; j < num_city+1; j++){
            WTA[i][j] = (i-1) * num_city + (j-1);
        }
    }

    int i, j;
    for(i = 0; i < num_neurons[side_v]; i++) {
        potential[side_v][i] = 0.0;
        threshold[side_v][i] = 1;
        last_spk[side_v][i] = -1;
        last_spk_st[side_v][i] = -1;
        last_spk_in[side_v][i] = -1;
    }

    for(i = 0; i < num_neurons[side_h]; i++) {
        potential[side_h][i] = 0.0;
        threshold[side_h][i] = 1;
        last_spk[side_h][i] = -1;
        last_spk_st[side_h][i] = -1;
        last_spk_in[side_h][i] = -1;
    }

    for (int i = 0; i < num_city; i++){
        potential[i] = new double[num_city];
        threshold[i] = new double[num_city];
        last_spk[i] = new double[num_city];
        last_spk_st[i] = new double[num_city];
        last_spk_in[i] = new double[num_city];
    }

    for (int i = 0; i < num_city; i++){
        for (int j = 0; j < num_city; j++){
            potential[i][j] = 0.0;
            threshold[i][j] = 1;
            last_spk[i][j] = -1;
            last_spk_st[i][j] = -1;
            last_spk_in[i][j] = -1;
        }
    }

  const std::vector<std::vector<double>> distance_matrix{
      {0, 2451, 713, 1018, 1631, 1374, 2408, 213, 2571, 875, 1420, 2145, 1972},
      {2451, 0, 1745, 1524, 831, 1240, 959, 2596, 403, 1589, 1374, 357, 579},
      {713, 1745, 0, 355, 920, 803, 1737, 851, 1858, 262, 940, 1453, 1260},
      {1018, 1524, 355, 0, 700, 862, 1395, 1123, 1584, 466, 1056, 1280, 987},
      {1631, 831, 920, 700, 0, 663, 1021, 1769, 949, 796, 879, 586, 371},
      {1374, 1240, 803, 862, 663, 0, 1681, 1551, 1765, 547, 225, 887, 999},
      {2408, 959, 1737, 1395, 1021, 1681, 0, 2493, 678, 1724, 1891, 1114, 701},
      {213, 2596, 851, 1123, 1769, 1551, 2493, 0, 2699, 1038, 1605, 2300, 2099},
      {2571, 403, 1858, 1584, 949, 1765, 678, 2699, 0, 1744, 1645, 653, 600},
      {875, 1589, 262, 466, 796, 547, 1724, 1038, 1744, 0, 679, 1272, 1162},
      {1420, 1374, 940, 1056, 879, 225, 1891, 1605, 1645, 679, 0, 1017, 1200},
      {2145, 357, 1453, 1280, 586, 887, 1114, 2300, 653, 1272, 1017, 0, 504},
      {1972, 579, 1260, 987, 371, 999, 701, 2099, 600, 1162, 1200, 504, 0},
  };

  cout << distance_matrix[0][1] << endl;

  return 0;
}
