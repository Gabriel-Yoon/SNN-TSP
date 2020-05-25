#include "core.hpp"
#include "simulation_parameters.hpp"

using namespace std;

core::core()
{
    // Number of Neurons
    neurons_visible_city = num_city * num_city;
    neurons_hidden_city = num_city * num_city;

    neurons_visible_bias = 0;
    neurons_hidden_bias = 0;
    
    num_neurons[side_v] = neurons_visible_city;
    num_neurons[side_h] = neurons_hidden_city;
    
    num_neurons_bias[0] = neurons_visible_bias;
    num_neurons_bias[1] = neurons_hidden_bias;

    // TSP dataset from "https://developers.google.com/optimization/routing/tsp#c++"
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

}

void sm_core::initialize()
{
    // Weight matrix, will be used
    weight_matrix.resize(num_neurons[side_v]);
    max_weight.resize(num_neurons[side_v]);
    min_weight.resize(num_neurons[side_v]);
    wt_delta_g_set.resize(num_neurons[side_v]);
    wt_delta_g_reset.resize(num_neurons[side_v]);
    for (int i = 0; i < num_neurons[side_v]; i++)
    {
        weight_matrix[i].resize(num_neurons[side_h]);
        max_weight[i].resize(num_neurons[side_h]);
        min_weight[i].resize(num_neurons[side_h]);
        wt_delta_g_set[i].resize(num_neurons[side_h]);
        wt_delta_g_reset[i].resize(num_neurons[side_h]);
    }
    if (rng_wt_set || rng_wt_reset)
    {
        weight_matrix_ideal.resize(num_neurons[side_v]);
        for (int i = 0; i < num_neurons[side_v]; i++)
        {
            weight_matrix_ideal[i].resize(num_neurons[side_h]);
        }
    }

    // Neuron Info, will only be used in the hidden side of the neuron
    // because the visible side will only act as the external injection purpose.
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

    wsum[side_v] = new double[num_neurons[side_v]];
    wsum[side_h] = new double[num_neurons[side_h]];

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
}

void sm_core::weight_load(int cell_type, char *fweight)
{

}

template <int is_spk, int is_rng> void sm_core::run_loop(double tnow, double tpre, sm_spk &spk_now, int which_spk, double &simtick, int &new_spk)
{
    EVENT_TIME_DUMP(tnow);
    if (is_spk)
    {
        if (param.enable_learning)
            weight_update(tnow);
    }

    potential_update_by_leak(tnow - tpre);
    export_ptn(tnow);

    if (is_spk)
    {
        potential_update_by_spk(spk_now, which_spk);
        export_ptn(tnow);
        sm_spk *spk_export;
        if (which_spk == spk_type_ext)
        {
            spk_export = queue_ext.top().second;
            export_spk(*spk_export, spk_type_ext);
        }
        else if (which_spk == spk_type_int)
        {
            spk_export = queue_spk.top().second;
            export_spk(*spk_export, spk_type_int);
        }
        else if (which_spk == spk_type_both)
        {
            spk_export = queue_ext.top().second;
            export_spk(*spk_export, spk_type_ext);
            spk_export = queue_spk.top().second;
            export_spk(*spk_export, spk_type_int);
        }
    }

    if (is_rng)
    {
        if (param.enable_random_walk)
        {
            potential_update_by_random_walk();
            export_ptn(tnow);
        }
        else if (param.enable_stochastic_vth)
        {
            threshold_update_stochastic();
            s
        }
        simtick += param.timestep_rng;
    }

    new_spk = compare_threshold(tnow);
}

double sm_core::run()
{

    double tend = param.num_of_label * param.num_of_image * phase->get_time_per_image();
    double tnow = 0.0;
    double injection_tick = param.timestep_injection;
    double simtick = param.timestep_rng;

    sm_spk *spk_now;
    int which_spk = 0;
    int is_spk = 0;
    int new_spk = 0;

    long int loop_count = 0;

    get_spk(&spk_now, &which_spk);

    cout << setprecision(9);

    while (1)
    {

#ifdef DEBUG_LOOP
        cout << "------LOOP------" << endl;
#endif
        // get spike event
        if (is_spk)
        {
            if (which_spk == spk_type_ext)
            {
                delete queue_ext.top().second;
                queue_ext.pop();
            }
            else if (which_spk == spk_type_int)
            {
                delete queue_spk.top().second;
                queue_spk.pop();
            }
            else if (which_spk == spk_type_both)
            {
                delete queue_ext.top().second;
                queue_ext.pop();
                delete queue_spk.top().second;
                queue_spk.pop();
            }
        }

        if (new_spk || is_spk)
        {
            new_spk = 0;
            is_spk = 0;
            get_spk(&spk_now, &which_spk);
        }

#ifdef DEBUG_LOOP
        cout << "simtick " << simtick << " spk time " << spk_now->time << endl;
#endif
        if (fabs(simtick - spk_now->time) < FLOAT_EPSILON_TIME)
        { // simtick == spk_now.time
            tnow = simtick;
            if (tnow > tend)
                break;
            is_spk = 1;
            if (spk_now->reset == true)
            {
                potential_reset(*spk_now); // Reset before or after run_loop()???
                run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else if (spk_now->st == true)
            {
                last_spk_st_update(*spk_now);
                run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
            else
            {
                run_loop<1, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }
        else if (simtick < spk_now->time)
        {
            tnow = simtick;
            if (tnow > tend)
                break;
            is_spk = 0;
            run_loop<0, 1>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
            tpre = tnow;
        }
        else
        { // if simtick > spk_now.time
            tnow = spk_now->time;
            if (tnow > tend)
                break;
            is_spk = 1;
            if (spk_now->reset == true)
            {
                potential_reset(*spk_now);
            }
            else if (spk_now->st == true)
            {
                last_spk_st_update(*spk_now);
            }
            else
            {
                run_loop<1, 0>(tnow, tpre, *spk_now, which_spk, simtick, new_spk);
                tpre = tnow;
            }
        }

        loop_count++;
    }

    cout << "Save final wij matrix to wij_last.bin" << endl;
    if (!param.enable_gpgm)
    {
        weight_save(wij_gp, "wij_last.bin");
    }
    else
    {
        weight_save(wij_gp, "wij_last_gp.bin");
        weight_save(wij_gm, "wij_last_gm.bin");
    }
    cout << "loop_count " << loop_count << endl;

    return tnow;
}

/*
void sm_core::weight_save(int cell_type, string filename) {
    ofstream os;
    os.open(filename, ios::binary);

    for(int i = 0; i < num_neurons[side_v]; i++) {
        for(int j = 0; j < num_neurons[sode_h]; j++) {
            if(cell_type == wij_gp) {
                os.write((char*)&(weight_matrix[i][j].Gp), sizeof(double));
            } else {
                os.write((char*)&(weight_matrix[i][j].Gm), sizeof(double));
            }
        }
    }

    if(param.wt_Delta_g_set_sigma_cycle) {
        for(int i = 0; i < num_neurons[side_V]; i++) {
            for(int j = 0; j < num_neurons[side_h]; j++) {
                if(cell_type == wij_gp) {
                    os.write((char*)&(weight_matrix_ideal[i][j].Gp), sizeof(double));
                } else {
                    os.write((char*)&(weight_matrix_ideal[i][j].Gm), sizeof(double));
                }
            }
        }
    }

    os.close();
}

#ifdef DEBUG_LOOP
#define EVENT_TIME_DUMP(t)          printf("event_now.time: %.09f\n", t)
#else
#define EVENT_TIME_DUMP(t)
#endif
*/