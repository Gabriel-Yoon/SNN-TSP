#include <math.h>
#include <algorithm>
//#include <unistd.h>

#include "core.h"

enum : int {
    data_phase,
    model_phase
};

spike *spk_null;

core::core(const char* param_file) : params(param_file)
{
    _numCity = 5;

	num_neurons[side_v] = _numCity* _numCity;
    num_neurons[side_h] = _numCity* _numCity;

    std::string cpu_str = "core0.";
    export_ptn_file[0] = cpu_str + "export_ptn_file_v"+ "ptn_v.dat";
    export_ptn_file[1] = cpu_str + "export_ptn_file_h"+ "ptn_h.dat";

}

void core::initialize(){

    // Neuron Layer Setting
    std::cout << "[START]Neuron Layer Setting" << std::endl;
    visibleLayer._neurons.resize(num_neurons[side_v]);
    visibleLayer.ManualSet(params);
    hiddenLayer._neurons.resize(num_neurons[side_h]);
    hiddenLayer.ManualSet(params);
    std::cout << "Visible Layer Neurons : " << visibleLayer._neurons.size()<< std::endl;
    std::cout << "Hidden Layer Neurons : " << hiddenLayer._neurons.size()<< std::endl;
    std::cout << "[_END_]Neuron Layer Setting" << std::endl;

    // Synapse Array Setting
    std::cout << "[START]Synapse Array Setting" << std::endl;
    synapseArray.setSynapseSize(num_neurons[side_v], num_neurons[side_h]);
    synapseArray.callSynapseArrayGpGm("/Users/gabriel/Development/SNN-TSP/src/build/weight.json");
    synapseArray.inspectWeightValues(params.min_weight, params.max_weight);
    std::string save_file_name = "core_synapse_weight";
    synapseArray.saveSynapseArrayGpGm(save_file_name);
    std::cout << "[_END_]Synapse Array Setting" << std::endl;

    // Load Magazine
    //loadMagazine("external_spike.json");
    
}

void core::potentialUpdate(spike& run_spike){
    std::cout << "Potential Update" << std::endl;
    if(run_spike._side == side_v){           // spike is from side_v
        if(params.enable_gpgm){
            for(int i = 0; i < synapseArray._synapses[run_spike._neuronNum].size(); i++){
                if (hiddenLayer._neurons[i]._active){
                    hiddenLayer._neurons[i].add_memV(synapseArray.delta_G(run_spike._neuronNum, i));
                }
            }
        }
    } else{                                 // spike is from side_h
        if(params.enable_gpgm){
            for(int i = 0; i < synapseArray._synapses.size(); i++){
                if (visibleLayer._neurons[i]._active){
                    visibleLayer._neurons[i].add_memV(synapseArray.delta_G(i, run_spike._neuronNum));
                }   
            }
        }
    }
}

void core::STDP(){

}

/*
void core::initialize_export() {

    ofs_spk_int = new ofstream[4];
    ofs_spk_ext = new ofstream[4];
    export_file = new string[4];
    if(export_spk_en) {
        ofs_spk_int[0].open(export_spk_file[0].c_str(), ios::binary); //int v x
        ofs_spk_int[1].open(export_spk_file[1].c_str(), ios::binary); //int v y
        ofs_spk_int[2].open(export_spk_file[2].c_str(), ios::binary); //int h x
        ofs_spk_int[3].open(export_spk_file[3].c_str(), ios::binary); //int h y
        ofs_spk_ext[0].open(export_spk_file[0].c_str(), ios::binary); //ext v x
        ofs_spk_ext[1].open(export_spk_file[1].c_str(), ios::binary); //ext v y
        ofs_spk_ext[2].open(export_spk_file[2].c_str(), ios::binary); //ext h x
        ofs_spk_ext[3].open(export_spk_file[3].c_str(), ios::binary); //ext h y
    }

    ofs_ptn_v = new ofstream;
    ofs_ptn_h = new ofstream;
    if(export_ptn_en){
        ofs_ptn_v->open(export_ptn_file[0].c_str(), ios::binary);
        ofs_ptn_h->open(export_ptn_file[1].c_str(), ios::binary);
    }

}
*/

void core::print_params() {
	// Should add neuron info, synapse info, etc.
	
	std::cout << "Simulation parameter list" << std::endl;
	std::cout << "--------------------------neuron------------------------ " << std::endl;
	std::cout << "neurons_v_data	: " << params.neurons_visible_data << std::endl;
	std::cout << "neurons_v_label	: " << params.neurons_visible_label << std::endl;
	std::cout << "neurons_h_data	: " << params.neurons_hidden_data << std::endl;
	std::cout << "neurons_v_bias	: " << params.neurons_visible_bias << std::endl;
	std::cout << "neurons_h_bias	: " << params.neurons_hidden_bias << std::endl;
	std::cout << "--------------------------time-------------------------- " << std::endl;
	std::cout << "timestep 			: " << params.timestep << std::endl;
	std::cout << "timestep_rng 		: " << params.timestep_rng << std::endl;
	std::cout << "refractory time	: " << params.refractory_time << std::endl;
	std::cout << "steps_transition	: " << params.steps_transition << std::endl;
	std::cout << "steps_data		: " << params.steps_data << std::endl;
	std::cout << "steps_model		: " << params.steps_model << std::endl;
	std::cout << "----------------------pulse delay----------------------- " << std::endl;
	std::cout << "delay_spikein2out			: " << params.delay_spikein2out << std::endl;
	std::cout << "wlr_width					: " << params.wlr_width << std::endl;
	std::cout << "delay_spikeout2wup		: " << params.delay_spikeout2wup << std::endl;
	std::cout << "delay_spikeout2wup_data	: " << params.delay_spikeout2wup_data << std::endl;
	std::cout << "delay_spikeout2wup_model	: " << params.delay_spikeout2wup_model << std::endl;
	std::cout << "delay_spikeout2td3		: " << params.delay_spikeout2td3 << std::endl;
	std::cout << "tset_width				: " << params.tset_width << std::endl;
	std::cout << "treset_width				: " << params.treset_width << std::endl;
	std::cout << "--------------------------mode-------------------------- " << std::endl;
	std::cout << "enable_random_walk 	:" << params.enable_random_walk << std::endl;
	std::cout << "enable_white_noise 	:" << params.enable_white_noise << std::endl;
	std::cout << "enable_sigmoid 		:" << params.enable_sigmoid << std::endl;
	std::cout << "enable_ps2 			:" << params.enable_ps2 << std::endl;
	std::cout << "enable_s2m 			:" << params.enable_s2m << std::endl;
	std::cout << "enable_sns 			:" << params.enable_sns << std::endl;
	std::cout << "is_real_data 			:" << params.is_real_data << std::endl;
	std::cout << "is_synaptic_delay 	:" << params.is_synaptic_delay << std::endl;
	std::cout << "is_neuron_delay 		:" << params.is_neuron_delay << std::endl;
	std::cout << "inference_by_label 	:" << params.inference_by_label << std::endl;
	std::cout << "--------------------------other------------------------- " << std::endl;
	std::cout << "data, label spike_rate 	:" << params.spike_rate << std::endl;
	std::cout << "bias_rate_factor 			:" << params.bias_rate_factor << std::endl;
	std::cout << "bias spike 				:" << params.spike_rate * params.bias_rate_factor << std::endl;
	std::cout << "wt_delta_g_set 			:" << std::fixed << params.wt_delta_g_set << std::endl;
	std::cout << "wt_delta_g_reset 			:" << std::fixed << params.wt_delta_g_reset << std::endl;
	std::cout << "pt_threshold 				:" << params.pt_threshold << std::endl;
	
	// Should add ON/OFF mode to print below
	/*
	if (params.inference_by_label) {
		std::cout << "blocked_area : " << params.blocked_area << std::endl;
	}
	if (params.enable_sigmoid) {
		std::cout << "sigmoid_mid : " << params.sigmoid_mid << std::endl;
		std::cout << "sigmoid_temp : " << params.sigmoid_temp << std::endl;
	}
	if (params.enable_ps2) {
		std::cout << "ps2_probability : " << params.ps2_probability << std::endl;
	}
	if (params.enable_s2m) {
		std::cout << "s2m_probability : " << params.s2m_probability << std::endl;
	}
	if (params.is_real_data) {
		std::cout << "conductance_level : " << params.conductance_level << std::endl;
	}
	if (params.enable_sns) {
		std::cout << "synapse_noise_sigma : " << params.synapse_noise_sigma << std::endl;
	}
	if (params.is_synaptic_delay) {
		std::cout << "synaptic_delay_mu : " << params.synaptic_delay_mu << std::endl;
		std::cout << "synaptic_delay_sigma : " << params.synaptic_delay_sigma << std::endl;
	}
	if (params.is_neuron_delay) {
		if (params.delay_method == "gauss") {
			std::cout << "neuron_delay_mu : " << params.neuron_delay_mu << std::endl;
			std::cout << "neuron_delay_sigma : " << params.neuron_delay_sigma << std::endl;
		}
		else if (params.delay_method == "exponential") {
			std::cout << "neuron_delay_exp_mean : " << params.neuron_delay_exp_mean << std::endl;
		}
		else {
			assert(false);
		}
	}
	*/
}

void core::ext_spike_load(double tend) {

}

std::tuple<np::array_2d<uint8_t>, np::array_2d<int8_t>> core::load_mnist_28(std::string dataset = "training", np::array_1d<int> digits = np::arange(10))
{
	std::string fname_img, fname_lbl;
	if (dataset == "training") {
		fname_img = "../../../train-images.idx3-ubyte";
		fname_lbl = "../../../train-labels.idx1-ubyte";
	}
	else if (dataset == "testing") {
		fname_img = "../../../t10k-images.idx3-ubyte";
		fname_lbl = "../../../t10k-labels.idx1-ubyte";
	}
	else {
		throw std::invalid_argument("dataset must be 'testing' or 'training'");
	}

	std::ifstream flbl(fname_lbl, std::ifstream::in | std::ifstream::binary);
	if (flbl.good() != true) {
		std::cout << "file open error: " << fname_lbl << std::endl;
		exit(EXIT_FAILURE);
	}
	uint32_t magic_nr = fs::read_uint32_be(flbl);
	uint32_t size = fs::read_uint32_be(flbl);
	(void)magic_nr;
#if 0
	std::cout << "magic_nr: " << magic_nr << " size: " << size << std::endl;
#endif
	std::vector<int8_t> lbl = fs::read_all_int8(flbl);
#if 0
	for (int8_t v : lbl) {
		printf("%d, ", v);
	}
	printf("\n");
#endif
	flbl.close();

	std::ifstream fimg(fname_img, std::ifstream::in | std::ifstream::binary);
	if (fimg.good() != true) {
		std::cout << "file open error: " << fname_img << std::endl;
		exit(EXIT_FAILURE);
	}
	magic_nr = fs::read_uint32_be(fimg);
	size = fs::read_uint32_be(fimg);
	uint32_t rows = fs::read_uint32_be(fimg);
	uint32_t cols = fs::read_uint32_be(fimg);
#if 0
	std::cout << "magic_nr: " << magic_nr << " size: " << size
		<< " rows: " << rows << " cols: " << cols << std::endl;
#endif
	std::vector<uint8_t> img = fs::read_all_uint8(fimg);
#if 0
	for (uint8_t v : img) {
		printf("%d, ", v);
	}
	printf("\n");
	exit(EXIT_FAILURE);
#endif
	fimg.close();

	std::vector<uint32_t> ind;
	for (uint32_t k = 0; k < size; k++) {
		if (std::find(digits.begin(), digits.end(), lbl[k]) != digits.end()) {
			ind.push_back(k);
		}
	}
	unsigned int N = ind.size();

#if 0
	std::cout << "N: " << ind.size() << std::endl;
	for (uint32_t v : ind) {
		printf("%d, ", v);
	}
	exit(EXIT_FAILURE);
#endif

	auto images = np::zeros<uint8_t>(N, rows * cols);
	auto labels = np::zeros<int8_t>(N, 1);

	for (unsigned int i = 0; i < ind.size(); i++) {
		images[i].clear();
		images[i].reserve(rows * cols);
		images[i].insert(images[i].begin(), img.begin() + ind[i] * rows * cols,
			img.begin() + (ind[i] + 1) * rows * cols);
		labels[i][0] = lbl[ind[i]];
	}

	return { images, labels }; // image : [# of image,784]  label : [# of image,1]
}

void core::generateMagazine(double tend){
    std::cout << "[START] Injection Magazine" << std::endl;
    double timestep_injection = 10e-6;
    int neuron = 0; // start city : if city 1 is the start, neuron =0;
    double time;
    double injection_step = tend / timestep_injection;

    auto result = nlohmann::json{
        {"time", json::array()},
        {"side", json::array()},
        {"neuron", json::array()},
    };

    for (int i = 0; i < injection_step; i++) {
        auto& _spikeTime = result["time"];
        auto& _side = result["side"];
        auto& _neuronNum = result["neuron"];

        _spikeTime.push_back(i*timestep_injection);
        _side.push_back(side_v);
        _neuronNum.push_back(neuron);

    }

    std::ofstream out("magazine_injection.json");
    out << result;

    std::cout << "[_END_] Injection Magazine" << std::endl;
}

void core::loadMagazine(const char* mag_file){   // load spikes into visibleMagazine

    std::cout << "[START] Load Magazine" << std::endl;
    SpikeMagazine tempMagazine;

    // magazine_file == "magazine.json"
    std::ifstream f(mag_file);
    json _magFile = json::parse(f);

    for(int i = 0; i < _magFile["time"].size(); i++){
        spike *_spike = new spike;
        // make a single spike;
        _spike->_spikeTime = _magFile["time"][i];
        // if (_spike->_spikeTime > mag_end_time) break;
        _spike->_spk.push_back(std::make_pair(_magFile["side"][i], _magFile["neuron"][i]));
        _spike->_reset = true;
        _spike->_st = true;
        //save a spike to tempMagazine
        tempMagazine.push(make_pair(_spike->_spikeTime, _spike));
    }

    while(!tempMagazine.empty()) {
        spike *_spike = new spike;
        _spike = tempMagazine.top().second;
        tempMagazine.pop();

        //Find spikes at the same timing
        while(!tempMagazine.empty()) {
            spike *spike_next = tempMagazine.top().second;
            if(_spike->_spikeTime != spike_next->_spikeTime) { break; }
            _spike->_spk.push_back(make_pair(spike_next->_side, spike_next->_neuronNum));
            delete spike_next;
            tempMagazine.pop();
        }

        visibleMagazine.push(make_pair(_spike->_spikeTime, _spike));

    }

    std::cout << "[_END_] Load Magazine" << std::endl;

}			

void core::setRandomWalkSchedule(double tend, int side, RandomWalkSchedule RandomWalkSchedule){	
    // set random walk schedule. use annealing schedule here
    cout << "[START] Setting Random Walk Schedule" << endl;
    double timestep_randomwalk = 10e-6;
    double time;
    double injection_step = tend / timestep_randomwalk;
    
    // Random Walk distribued evenly with the spacing of timestep_randomwalk
    for (int i = 0; i < injection_step; i++) {
        random_walk* rw = new random_walk;
        rw->_time = i * timestep_randomwalk;
        rw->_randomWalk.first = side; // which side to random walk
        rw->_randomWalk.second = 0.06; //
        RandomWalkSchedule.push(make_pair(rw->_time, rw));
    }

    // make one for the specific function (ex. Simulated Annealing!)
}

int core::assignTask(spike **run_spike, double tpre, double tnow, int magazine_side) {
    // return false for random walk, true for spike event (reset, st, dummy, real spike event)
    
    // First you should perform Leak function. MAKE IT ASAP!
    
    enum case_id : int {
        visible_spike,
        hidden_spike,
        visible_random_walk,
        hidden_random_walk
    };

    const int size = 4;
    double time[size];
    int case_id;

    auto get_tnext = [&](auto _queue0, auto _queue1, auto _queue2, auto _queue3) {
        time[0] = (!_queue0.empty()) ? _queue0.top().first : -1;
        time[1] = (!_queue1.empty()) ? _queue1.top().first : -1;
        time[2] = (!_queue2.empty()) ? _queue2.top().first : -1;
        time[3] = (!_queue3.empty()) ? _queue3.top().first : -1;
        double minVal = std::numeric_limits<double>::max();
        for(int i = 0; i < size; i++){
            if(time[i] != -1 && time[i] < minVal) {
                minVal = time[i];
                case_id = i;
            }
        }
        return minVal;
    };

    tpre = tnow;
    tnow = get_tnext(visibleMagazine, hiddenMagazine, visibleRandomWalkSchedule, hiddenRandomWalkSchedule);
    std::cout << "tpre : " << tpre << " | tnow : " << tnow << std::endl;

    switch(case_id)
    {
        case visible_spike:
            std::cout << "visible_spike" << std::endl;
            *run_spike = visibleMagazine.top().second;
            delete visibleMagazine.top().second;
            visibleMagazine.pop();
            magazine_side = side_v;
            return 1;
        case hidden_spike:
            std::cout << "hidden_spike" << std::endl;
            *run_spike = hiddenMagazine.top().second;
            delete hiddenMagazine.top().second;
            hiddenMagazine.pop();
            magazine_side = side_h;
            return 1;
        case visible_random_walk:
            std::cout << "visible_random_walk" << std::endl;
            visibleLayer.RandomWalk();
            magazine_side = side_v;
            return 0;
        case hidden_random_walk:
            std::cout << "hidden_random_walk" << std::endl;
            hiddenLayer.RandomWalk();
            magazine_side = side_h;
            return 0;
        default:
            std::cout << "No event scheduled" << std::endl;
            return -1;
    }

}

void core::shootSpike(spike& run_spike, int& phase){    

    SpikeMagazine selfMagazine;
    SpikeMagazine targetMagazine;

    spike *spikeCasing = new spike(run_spike);
    // 1. To self magazine
    // 1-1. Create ST_PAUSE Event
    if(params.hw_ISO_MOD) {
        std::cout << "Generate ST_PAUSE event" << std::endl;
        spike *run_spike_st_pause = new spike(*spikeCasing);
        run_spike_st_pause->_spikeTime = run_spike._spikeTime;
        run_spike_st_pause->_reset = false;
        run_spike_st_pause->_st = true;
        selfMagazine.push(make_pair(run_spike._spikeTime, run_spike_st_pause));
        // delete run_spike_st_pause;
    }

    // 1-2. Create Reset Event
    if(params.hw_RES_EN) {
        std::cout << "Generate Reset event" << std::endl;
        spike *run_spike_reset = new spike(*spikeCasing);
        run_spike_reset->_spikeTime = run_spike._spikeTime + params.delay_spikein2out;
        run_spike_reset->_reset = true;
        run_spike_reset->_st = false;
        selfMagazine.push(make_pair(run_spike._spikeTime + params.delay_spikein2out, run_spike_reset));
        // delete run_spike_reset;
    }

    // 1-3. Create Dummy Event
    std::cout << "Generate Dummy event" << std::endl;
    spike *run_spike_dummy = new spike(*spikeCasing);
    run_spike_dummy->_spikeTime = run_spike._spikeTime + params.refractory_time + FLOAT_EPSILON_TIME;
    run_spike_dummy->_reset = true;
    run_spike_dummy->_st = false;
    selfMagazine.push(make_pair(run_spike._spikeTime + params.refractory_time + FLOAT_EPSILON_TIME, run_spike_dummy));
    // delete run_spike_dummy;

    // 2. To target magazine
    std::cout << "Shoot spike!" << std::endl;
    spike *run_spike_target = new spike(run_spike);
    if(!params.enable_gpgm) {
        run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr + params.wlr_width;
        run_spike_target->_reset = false;
        run_spike_target->_st = false;
        std::cout << "ERROR HERE?" << std::endl;
        targetMagazine.push(make_pair(run_spike_target->_spikeTime, run_spike_target));
        std::cout << "OR HERE?" << std::endl;
    } else { // enable _ gpgm
        if(phase == data_phase) {
            if(run_spike._side == side_v){
                run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_data_v + params.wlr_width;
                std::cout << "Target on-site1" << std::endl;
            } else {
                run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_data_h + params.wlr_width;
                std::cout << "Target on-site2" << std::endl;
            }
        } else {
            if(run_spike._side == side_h){
                run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_model_v + params.wlr_width;
                std::cout << "Target on-site3" << std::endl;
            } else {
                run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_model_h + params.wlr_width;
                std::cout << "Target on-site4" << std::endl;
            }
        }
        targetMagazine.push(make_pair(run_spike_target->_spikeTime, run_spike_target));
    }

    while(!selfMagazine.empty()){
        spike *new_spike = selfMagazine.top().second;
        if(run_spike._side == side_v){
            visibleMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        } else {
            std::cout << "-----HIDDEN SPIKE GENERATED!!-----" << std::endl;
            hiddenMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
        selfMagazine.pop();
    }

    while(!targetMagazine.empty()){
        spike *new_spike = targetMagazine.top().second;
        if(run_spike._side == side_v){
            hiddenMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        } else {
            visibleMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
        targetMagazine.pop();
    }

    // delete selfMagazine;
    // delete targetMagazine;
}

void core::reloadSpike(double tnow) {
    
    for(int i = 0; i<visibleLayer._neurons.size(); i++){
        if(visibleLayer._neurons[i]._memV >= visibleLayer._neurons[i]._Vth){
            spike *new_spike = new spike;
            new_spike->_spikeTime = tnow;
            new_spike->_side = side_v;
            new_spike->_neuronNum = i;
            new_spike->_reset = true;
            new_spike->_st = true;
            visibleMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
    }

    for(int i = 0; i<hiddenLayer._neurons.size(); i++){
        if(hiddenLayer._neurons[i]._memV >= hiddenLayer._neurons[i]._Vth){
            spike *new_spike = new spike;
            new_spike->_spikeTime = tnow;
            new_spike->_side = side_h;
            new_spike->_neuronNum = i;
            new_spike->_reset = true;
            new_spike->_st = true;
            hiddenMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
    }
    // compare threshold
    // use loadSpike from spike_magazine
}

void core::run_simulation(){
    
    double tend = 1.0;
    double tnow = 0.0;
    double tpre = 0.0;

    spike *run_spike;
    int task;
    int magazine_side;
    int phase = data_phase;
    long int loop_count = 0;

    generateMagazine(tend);
    loadMagazine("/Users/gabriel/Development/SNN-TSP/src/build/magazine_injection.json");
    while(1){
        std::cout << "[START] Assign Task" << std::endl;
        task = assignTask(&run_spike, tpre, tnow, magazine_side);
        std::cout << "[_END_] Assign Task" << std::endl;

        if(task == 1) { // (_reset, _st) = ( , )
            if (run_spike->_reset) { // (1,-)
                if (run_spike->_st) {                           // (1,1)
                    std::cout << "(1,1) Shoot Spike" << std::endl;
                    shootSpike(*run_spike, phase);
                } else {                                        // (1,0)
                    std::cout << "(1,0) Reset Event" << std::endl;
                    for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                        if(it->first == side_v){
                            visibleLayer._neurons[it->second].memV_Reset();
                        } else {
                            hiddenLayer._neurons[it->second].memV_Reset();
                        }
		            }
                }
            } else{ // (0,-)
                if (run_spike->_st) {                           // (0,1)
                    std::cout << "(0,1) ST_PAUSE Event" << std::endl;
                    for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                        if(it->first == side_v){
                            visibleLayer._neurons[it->second].turnOFF();
                        } else {
                            hiddenLayer._neurons[it->second].turnOFF();
                        }
		            }
                } else {                                          // (0,0) but self magazine dummy event
                    std::cout << "(0,0) Event" << std::endl;
                    if (magazine_side == run_spike->_side) {
                        for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                            if(it->first == side_v){
                                visibleLayer._neurons[it->second].turnON();
                            } else {
                                hiddenLayer._neurons[it->second].turnON();
                            }
		                }
                    } else { // (0,0) but bullet from another magazine
                        potentialUpdate(*run_spike);
                    }
                }
            }
        } else if (task == -1){
            std::cout << "No more event!" << std::endl;
        }
        reloadSpike(tnow);

    }
}