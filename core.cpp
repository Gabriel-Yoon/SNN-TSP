#include "core.hpp"
#include "simulation_parameters.hpp"

using namespace std;

core::core()
{
    // Number of Neurons
    neurons_visible_data = 25;
    neurons_visible_bias = 0;
    neurons_hidden_data = 25;
    neurons_hidden_bias = 0;
    num_neurons[0] = (neurons_visible_data + neurons_visible_bias);
    num_neurons[1] = (neurons_hidden_data + neurons_hidden_bias);
    num_neurons_datalabel[0] = neurons_visible_data;
    num_neurons_datalabel[1] = neurons_hidden_data;
    num_neurons_bias[0] = neurons_visible_bias;
    num_neurons_bias[1] = neurons_hidden_bias;
}

void sm_core::initialize(char *fextspk, char *fexttime, char *fwij, char *fwij_gp, char *fwij_gm)
{

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

    potential[side_v] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_v], 32);
    potential[side_h] = (double *)_mm_malloc(sizeof(double) * num_neurons[side_h], 32);

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

    wsum[side_v] = new double[num_neurons_datalabel[side_v]];
    wsum[side_h] = new double[num_neurons_datalabel[side_h]];

    int i, j;
    for (i = 0; i < num_neurons[side_v]; i++)
    {
        potential[side_v][i] = 0.0;
        threshold[side_v][i] = 1;
        last_spk[side_v][i] = -1;
        last_spk_st[side_v][i] = -1;
        last_spk_in[side_v][i] = -1;
    }
    for (i = 0; i < num_neurons[side_h]; i++)
    {
        potential[side_h][i] = 0.0;
        threshold[side_h][i] = 1;
        last_spk[side_h][i] = -1;
        last_spk_st[side_h][i] = -1;
        last_spk_in[side_h][i] = -1;
    }
}

/*
int sm_core::get_spk(sm_spk **spk_now, int *which_spk) {
    int num_spk_ext = queue_ext.size();
    int num_spk_int = queue_spk.size();

    if(num_spk_ext == 0) {
        if(num_spk_int == 0) { // No More event
            *spk_now = spk_null;
            return -1;
        }
        *spk_now = queue_spk.top().second;
        *which_spk = spk_type_int;
        return 0;
    } else if(num_spk_int == 0) {
        *spk_now = queue_ext.top().second;
        *which_spk = spk_type_ext;
        return 0;
    }

    sm_spk *spk_ext = queue_ext.top().second;
    sm_spk *spk_int = queue_spk.top().second;
    if(fabs(spk_ext->time - spk_int->time) < FLOAT_EPSILON_TIME) {
        if((spk_ext->reset == spk_int->reset) && (spk_ext->st == spk_int->st)) {
            spk_ext->merge(*spk_int);
            *spk_now = spk_ext;
            *which_spk = spk_type_both;
        } else {
            *spk_now = spk_ext;
            *which_spk = spk_type_ext;
        }
    } else if(spk_ext->time < spk_int->time) {
        *spk_now = spk_ext;
        *which_spk = spk_type_ext;
    } else { // spk_ext->time > spk_int->time
        *spk_now = spk_int;
        *which_spk = spk_type_int;
    }

    return 0;
}
*/

void sm_core::ext_spike_load(char *fext, char *ftime)
{
    ifstream isidx, istime;
    isidx.open(fext, ios::binary);
    istime.open(ftime, ios::binary);

    if (isidx.fail())
    {
        cout << "Error opening file " << fext << ". Exit" << endl;
        exit(1);
    }
    if (istime.fail())
    {
        cout << "Error opening file " << ftime << ". Exit" << endl;
        exit(1);
    }

    char buf[20];
    char *ptr;
    int count = 0;
    int side;
    int neuron;
    double time;

    //priority_queue<sm_spk_one, vector<sm_spk_one>, sm_spk_one>queue_ext_pri;
    priority_queue<pair<double, sm_spk_one *>, vector<pair<double, sm_spk_one *>>,
                   greater<pair<double, sm_spk_one *>>>
        queue_ext_pri;

    while (1)
    {
        isidx.getline(buf, 20);
        if ((isidx.rdstate() & ifstream::eofbit) != 0)
        {
            break;
        }
        istime.read((char *)&time, sizeof(double));
        side = strtol(buf, &ptr, 10);
        ptr++;
        neuron = strtol(ptr, NULL, 10);

        sm_spk_one *spk = new sm_spk_one;
        spk->time = time;
        if (side == 0)
            spk->side = side_v;
        else
            spk->side = side_h;
        spk->neuron = neuron;
        queue_ext_pri.push(make_pair(time, spk));
        count++;
    }

    while (!queue_ext_pri.empty())
    {
        sm_spk_one *spk_one = queue_ext_pri.top().second;
        queue_ext_pri.pop();

        // Dummy event to kick compare_threshold at the end of refractory time
        double ref_end_time = spk_one->time + param.refractory_time + FLOAT_EPSILON_TIME;
        if (phase->query_phase_d_or_m(ref_end_time) == sm_model_phase)
        {
            sm_spk *spk_ref = new sm_spk;
            spk_ref->time = ref_end_time;
            queue_ext.push(make_pair(ref_end_time, spk_ref));
        }

        sm_spk *spk_ext = new sm_spk; // Actual spike event
        spk_ext->spk.push_back(make_pair(spk_one->side, spk_one->neuron));

        //Find spikes at the same timing
        while (!queue_ext_pri.empty())
        {
            sm_spk_one *spk_next = queue_ext_pri.top().second;
            if (spk_one->time != spk_next->time)
            {
                break;
            }
            spk_ext->spk.push_back(make_pair(spk_next->side, spk_next->neuron));
            delete spk_next;
            queue_ext_pri.pop();
        }

        // Create last_spk_st update event for ST_PAUSE
        sm_spk *spk_st_update = new sm_spk(*spk_ext);
        spk_st_update->time = spk_one->time;
        spk_st_update->st = true;
        queue_ext.push(make_pair(spk_st_update->time, spk_st_update));

        double time_fire2 = spk_one->time + param.delay_spikein2out;
        int phase_now = phase->query_phase_d_or_m(spk_one->time);

        // Create reset event
        if (param.hw_RES_EN)
        {
            sm_spk *spk_ext_reset = new sm_spk(*spk_ext);
            spk_ext_reset->time = time_fire2;
            spk_ext_reset->reset = true;
            queue_ext.push(make_pair(time_fire2, spk_ext_reset));
        }

        // Push spike event to queue
        if (!param.enable_gpgm)
        {
            spk_ext->time = time_fire2 + param.delay_spikeout2wlr + param.wlr_width;
        }
        else
        {
            if (phase_now == sm_data_phase)
            {
                spk_ext->time = time_fire2 + param.delay_spikeout2wlr_data + param.wlr_width;
            }
            else
            {
                spk_ext->time = time_fire2 + param.delay_spikeout2wlr_model + param.wlr_width;
            }
        }
        queue_ext.push(make_pair(spk_ext->time, spk_ext));

        // WUP event
        if (param.enable_learning)
        {
            if (!param.enable_gpgm)
            {
                sm_spk *spk_wup = new sm_spk(*spk_ext); // WUP event
                spk_wup->time = time_fire2 + param.delay_spikeout2wup;
                queue_wup_ext.push(make_pair(spk_wup->time, spk_wup));
            }
            else
            {
                sm_spk *spk_wup_v = new sm_spk;
                sm_spk *spk_wup_h = new sm_spk;
                for (auto it = spk_ext->spk.begin(); it != spk_ext->spk.end(); it++)
                {
                    if (it->first == side_v)
                    {
                        spk_wup_v->spk.push_back(*it);
                    }
                    else
                    {
                        spk_wup_h->spk.push_back(*it);
                    }
                }
                if (spk_wup_v->spk.size() > 0)
                {
                    if (phase_now == sm_data_phase)
                    {
                        spk_wup_v->time = time_fire2 + param.delay_spikeout2td3;
                    }
                    else
                    {
                        spk_wup_v->time = time_fire2 + param.delay_spikeout2td3 - param.tset_width;
                    }
                    queue_wup_ext.push(make_pair(spk_wup_v->time, spk_wup_v));
                }
                else
                {
                    delete spk_wup_v;
                }
                if (spk_wup_h->spk.size() > 0)
                {
                    if (phase_now == sm_data_phase)
                    {
                        spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_data;
                    }
                    else
                    {
                        spk_wup_h->time = time_fire2 + param.delay_spikeout2wup_model;
                    }
                    queue_wup_ext.push(make_pair(spk_wup_h->time, spk_wup_h));
                }
                else
                {
                    delete spk_wup_h;
                }
            }
        }

        delete spk_one;
    }
}

void sm_core::weight_load(int cell_type, char *fweight)
{
    ifstream is;
    is.open(fweight, ios::binary);
    if (is.fall())
    {
        cout << "Error opening file " << fweight << ". Exit." << endl;
        exit(1);
    }

    // Check file size
    is.seekg(0, ios::end);
    auto eofpos = is.tellg();
    is.clear();
    is.seekg(0, ios::beg);
    auto begpos = is.tellg();
    auto fsize = eofpos - begpos;
    auto dsize_wt = sizeof(double) * num_neurons[side_v] * num_neurons[side_h];
    auto dsize_st = sizeof(double) * num_neurons[side_v] * num_neurons[side_h];
    if ((fsize != dsize_wt) && (fsize != (dsize_wt + dsize_st)))
    {
        cout << "Unexpected file size " << fsize << ". Exit." << endl;
        exit(1);
    }

    double weight;

    for (int i = 0; i < num_neurons[side_v]; i++)
    {
        for (int j = 0; j < num_neurons[side_h]; j++)
        {
            is.read((char *)&weight, sizeof(double));
            if (cell_type == wij_gp)
            {
                if (weight > max_weight[i][j].Gp)
                {
                    weight = max_weight[i][j].Gp;
                }
                else if (weight < min_weight[i][j].gp)
                {
                    weight = min_weight[i][j].Gp;
                }
                weight_matrix[i][j].Gp = weight;
            }
            else
            {
                if (weight > max_weight[i][j].Gm)
                {
                    weight = max_weight[i][j].Gm;
                }
                else if (weight < min_weight[i][j].Gm)
                {
                    weight = min_weight[i][j].Gm;
                }
                weight_matrix[i][j].Gm = weight;
            }
        }
    }

    if (rng_wt_set || rng_wt_reset)
    {
        if (fsize == dsize_wt)
        {
            // First epoch. Set ideal weight
            for (int i = 0; i < num_neurons[side_v]; i++)
            {
                for (int j = 0; j < num_neurons[side_h]; j++)
                {
                    double var = rng_wt_set->get_val();
                    if (cell_type == wij_gp)
                    {
                        weight_matrix_ideal[i][j].Gp = weight_matrix[i][j].Gp + var;
                    }
                    else
                    {
                        weight_matrix_ideal[i][j].Gm = weight_matrix[i][j].Gm + var;
                    }
                }
            }
        }
        else
        {
            // Load ideal weight
            for (int i = 0; i < num_neurons[side_v]; i++)
            {
                for (int j = 0; j < num_neurons[side_h]; j++)
                {
                    is.read((char *)&weight, sizeof(double));
                    if (cell_type == wij_gp)
                    {
                        weight_matrix_ideal[i][j].Gp = weight;
                    }
                    else
                    {
                        weight_matrix_ideal[i][j].Gm = weight;
                    }
                }
            }
        }
    }

    is.close();
}

template <int is_spk, int is_rng>
void sm_core::run_loop(double tnow, double tpre, sm_spk &spk_now, int which_spk, double &simtick, int &new_spk)
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
    double tpre = 0.0;
    double simtick = param.timestep_rng;

    sm_spk *spk_now;
    int which_spk = 0;
    int is_spk = 0;
    int new_spk = 0;

    int sm_phase = sm_model_phase;

    long int loop_count = 0;

    if (!param.enable_gpgm)
    {
        weight_save(wij_gp, "wij_first.bin");
    }
    else
    {
        weight_save(wij_gp, "wij_first_gp.bin")
            weight_save(wij_gm, "wij_first_gm.bin")
    }
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