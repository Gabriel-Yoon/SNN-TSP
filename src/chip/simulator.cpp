#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>  
#include <time.h>
//#include <sys/time.h>
//#include <sys/resource.h>

//#include <windows.h> // Sleep �Լ�

#include "core.h"
#include "../utils/utils.h"

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
    /*
    clock_t start, end;
    double result;
    start = clock();
    system("color 0A");
    */

   double tend = 0.01;

    if (argc >= 2) {
        try {
            tend = std::stod(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid value for tend. Using the default value: " << tend << std::endl;
        }
    }

    char* chip_param_file_path = "sim_params.test.json";
    const std::string tsp_data_file_path = "tsp_data.json";
    core mycore(chip_param_file_path, tsp_data_file_path);
    //mycore.print_params();
    mycore.initialize();
    mycore.print_params();
    mycore.run_simulation(tend);
    
    std::cout << std::setprecision(4);
    std::cout << "simulation done" << std::endl;
    /*
    // Time print 1
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    cout << "User time " << usage.ru_utime.tv_sec + usage.ru_utime.tv_usec * 10e-7 << "sec" << endl;
    cout << "System time" << usage.ru_stime.tv_sec + usage.ru_utime.tv_usec * 10e-7 << "sec" << endl;
    cout << "maxurss " << usage.ru_maxrss / 1024 << " MB" << endl;

    // Time print 2
    end = clock();
    result = (double)(end - start);
    cout << "result : " << ((result) / CLOCKS_PER_SEC) << " seconds" << endl;
    printf("%f", result / CLOCKS_PER_SEC);
    */
    return 0;
}