#include <math.h>
#include <algorithm>
//#include <unistd.h>

#include "core.h"

enum : int {
    data_phase,
    model_phase
};

spike *spk_null;

core::core(const char* param_file) : params(param_file), _rng(params)
{
    _numCity = 5;

	num_neurons[side_v] = _numCity* _numCity;
    num_neurons[side_h] = _numCity* _numCity;

    potentialFilePath[side_v] = "VisibleMemV.json";
    potentialFilePath[side_h] = "HiddenMemV.json";

    std::string cpu_str = "core0.";

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
    exportSynapseWeightsToJson("weight_gp_gm.json");
    std::cout << "[_END_]Synapse Array Setting" << std::endl;

    // Make Spike SaveFile
    //makeSpikeFile("spikes");
    
}

void core::makeSpikeFile(std::string filename){
        auto result = nlohmann::json{
        {"time", json::array()},
        {"neuron", json::array()},
        };

        std::ofstream out(filename + ".json");
        out << result;
}

void core::writeSpikeIntoFile(spike& run_spike){
        std::string filepath = "/Users/gabriel/Development/SNN-TSP/src/build/spikes.json";
        std::ofstream out(filepath);
        
        auto result = nlohmann::json{
        {"time", json::array()},
        {"neuron", json::array()},
        };

        //std::ifstream ifs(filepath);
        //json _spikesavedfile = json::parse(ifs);
        
        auto& _time = result["time"];
        auto& _neuron = result["neuron"];

        for(auto it = run_spike._spk.begin(); it != run_spike._spk.end(); it++) { //            
            _time.push_back(run_spike._spikeTime);
            _neuron.push_back(it->second);
        }
        out << result;
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
	std::cout << "enable_gpgm        	:" << params.enable_gpgm << std::endl;
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
    std::cout << "Injection Magazine" << std::endl;
    double timestep_injection = 40e-6;
    int neuron = 0; // start city : if city 1 is the start, neuron =0;
    double time;
    double injection_step = tend / timestep_injection;

    auto result = nlohmann::json{
        {"time", json::array()},
        {"side", json::array()},
        {"neuron", json::array()},
    };

    for (int i = 1; i < injection_step; i++) {
        auto& _spikeTime = result["time"];
        auto& _side = result["side"];
        auto& _neuronNum = result["neuron"];

        _spikeTime.push_back(i*timestep_injection);
        _side.push_back(side_v);
        _neuronNum.push_back(neuron);

    }

    std::ofstream out("magazine_injection.json");
    out << result;
}

void core::loadMagazine(const char* mag_file){   // load spikes into visibleMagazine

    std::cout << "Load Magazine" << std::endl;
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
        visibleMagazine.push(make_pair(_spike->_spikeTime, _spike));
        
        //save a spike to tempMagazine
        //tempMagazine.push(make_pair(_spike->_spikeTime, _spike));
    }

    // while(!tempMagazine.empty()) {
    //     spike *_spike = new spike;
    //     _spike = tempMagazine.top().second;
    //     tempMagazine.pop();

    //     //Find spikes at the same timing
    //     // while(!tempMagazine.empty()) {
    //     //     spike *spike_next = tempMagazine.top().second;
    //     //     if(_spike->_spikeTime != spike_next->_spikeTime) { break; }
    //     //     _spike->_spk.push_back(make_pair(spike_next->_side, spike_next->_neuronNum));
    //     // }
    //     visibleMagazine.push(make_pair(_spike->_spikeTime, _spike));
    // }
}			

void core::setRandomWalkSchedule(double tend, int side){	
    // set random walk schedule. use annealing schedule here
    cout << "Setting Random Walk Schedule" << endl;
    double timestep_randomwalk = 10e-6;
    double time;
    double injection_step = tend / timestep_randomwalk;
    
    // Random Walk distribued evenly with the spacing of timestep_randomwalk
    for (int i = 0; i < injection_step; i++) {
        random_walk* rw = new random_walk;
        rw->_time = (i+1) * timestep_randomwalk;
        rw->_randomWalk.first = side; // which side to random walk
        rw->_randomWalk.second = 0.06; // manual or auto
        if (side == side_v){
            visibleRandomWalkSchedule.push(make_pair(rw->_time, rw));
        } else { // side_h
            hiddenRandomWalkSchedule.push(make_pair(rw->_time, rw));
        }
    }

    // make one for the specific function (ex. Simulated Annealing!)
}

int core::assignTask(spike **run_spike, double& tpre, double& tnow, double& tend, int* magazine_side) {
    
    if(tnow > tend){
        return -1;
    }

    enum case_id : int {
        visible_spike,
        hidden_spike,
        visible_random_walk,
        hidden_random_walk
    };

    const int size = 4;
    double time[size];
    int case_id;

    // compare latest task times in
    // visibleMagazine, hiddenMagazine, visibleRandomWalkSchedule, hiddenRandomWalkSchedule
    // to get the next task
    auto get_tnext = [&](auto _queue0, auto _queue1, auto _queue2, auto _queue3) {
        time[0] = (!_queue0.empty()) ? _queue0.top().first : -1;
        time[1] = (!_queue1.empty()) ? _queue1.top().first : -1;
        time[2] = (!_queue2.empty()) ? _queue2.top().first : -1;
        time[3] = (!_queue3.empty()) ? _queue3.top().first : -1;
        double minVal = std::numeric_limits<double>::max();
        for(int i = size-1; i >= 0; i--){
            if(time[i] != -1 && time[i] < minVal) {
                minVal = time[i];
                case_id = i;
                // if(time[i] == 0 || time[i] == 0.0000001 || time[i] <-100){
                //     std::cout << "wrong case_id = " << i << std::endl;
                // }
            }
        }
        return minVal;
    };

    tnow = get_tnext(visibleMagazine, hiddenMagazine, visibleRandomWalkSchedule, hiddenRandomWalkSchedule);

    // Leak function (Later add is_base_of to see if the neurons are based of lif_neurons)
    if (tnow != tpre){
        for(int i = 0; i < visibleLayer._neurons.size(); i++){
            visibleLayer._neurons[i].memV_Leak(tpre, tnow);
        }
        for(int i = 0; i < hiddenLayer._neurons.size(); i++){
            hiddenLayer._neurons[i].memV_Leak(tpre, tnow);
        }
        exportNeuronPotentialToJson(tnow);
    }

    switch(case_id)
    {
        case visible_spike:
            *run_spike = visibleMagazine.top().second;
            *magazine_side = side_v;
            return 0;
        case hidden_spike:
            *run_spike = hiddenMagazine.top().second;
            *magazine_side = side_h;
            return 1;
        case visible_random_walk:
            visibleLayer.RandomWalk(_rng);
            *magazine_side = side_v;
            return 2;
        case hidden_random_walk:
            hiddenLayer.RandomWalk(_rng);
            *magazine_side = side_h;
            return 3;
        default:
            std::cout << "SIMULATION END" << std::endl;
            return -1;       
    }
}

void core::shootSpike(spike& run_spike, int& phase){    
    for(auto it = run_spike._spk.begin(); it != run_spike._spk.end(); it++){
        
        writeSpikeIntoFile(run_spike);
        spikeRecorder.push_back(make_pair(run_spike._spikeTime, it->second));
        // TEMPORARY CODE
        // double check turn neuron OFF right away
        if(it->first == side_v){
            visibleLayer._neurons[it->second].turnOFF();
        } else {
            hiddenLayer._neurons[it->second].turnOFF();
        }

        // 1. To self magazine
        // 1-1. Create ST_PAUSE Event
        spike *run_spike_st_pause = new spike;
        if(params.hw_ISO_MOD) {
            run_spike_st_pause->_spikeTime = run_spike._spikeTime + FLOAT_EPSILON_TIME;
            run_spike_st_pause->_reset = false;
            run_spike_st_pause->_st = true;
            run_spike_st_pause->_spk.push_back(make_pair(it->first, it->second));
            if(params.enable_BM){ // WTA condition
                // int h_WTA = it->second / _numCity + 1;
                int h_city = it->second % _numCity;
                // Be aware of the index! the real city num is : h_city = it->second % _numCity + 1;
                for(int wta_num = 0; wta_num < _numCity; wta_num++){
                    int iso_city = h_city + wta_num * _numCity;
                    run_spike_st_pause->_spk.push_back(make_pair(side_h, iso_city));
                }
            }
        }

        // 1-2. Create Reset Event
        spike *run_spike_reset = new spike;
        if(params.hw_RES_EN) {    
            run_spike_reset->_spikeTime = run_spike._spikeTime + params.delay_spikein2out;
            run_spike_reset->_reset = true;
            run_spike_reset->_st = false;
        }

        // 1-3. Create Dummy Event
        spike *run_spike_dummy = new spike;
        run_spike_dummy->_spikeTime = run_spike._spikeTime + params.refractory_time + FLOAT_EPSILON_TIME;
        run_spike_dummy->_reset = false;
        run_spike_dummy->_st = false;
        run_spike_dummy->_spk.push_back(make_pair(it->first, it->second));
        if(params.enable_BM){ // WTA condition
            // int h_WTA = it->second / _numCity + 1;
            int h_city = it->second % _numCity;
            for(int wta_num = 0; wta_num < _numCity; wta_num++){
                int iso_city = h_city + wta_num * _numCity;
                run_spike_dummy->_spk.push_back(make_pair(side_h, iso_city));
            }
        }

        // 2. To target magazine
        spike *run_spike_target = new spike;
        if(!params.enable_gpgm) {
            run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr + params.wlr_width;
            run_spike_target->_reset = false;
            run_spike_target->_st = false;

        } else { // enable _ gpgm
            if(phase == data_phase) {
                if(it->second == side_v){
                    run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_data_v + params.wlr_width;
                } else {
                    run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_data_h + params.wlr_width;
                }
            } else {
                if(it->second == side_h){
                    run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_model_v + params.wlr_width;
                } else {
                    run_spike_target->_spikeTime = run_spike._spikeTime + params.delay_spikein2out + params.delay_spikeout2wlr_model_h + params.wlr_width;
                }
            }
        }
        run_spike_target->_spk.push_back(make_pair(it->first, it->second));

        if(it->first == side_v){
            visibleMagazine.push(make_pair(run_spike_st_pause->_spikeTime, run_spike_st_pause));
            visibleMagazine.push(make_pair(run_spike_dummy->_spikeTime, run_spike_dummy));
            visibleMagazine.push(make_pair(run_spike_reset->_spikeTime, run_spike_reset));
            hiddenMagazine.push(make_pair(run_spike_target->_spikeTime, run_spike_target));
        } else {
            hiddenMagazine.push(make_pair(run_spike_st_pause->_spikeTime, run_spike_st_pause));
            hiddenMagazine.push(make_pair(run_spike_reset->_spikeTime, run_spike_reset));
            hiddenMagazine.push(make_pair(run_spike_dummy->_spikeTime, run_spike_dummy));
            visibleMagazine.push(make_pair(run_spike_target->_spikeTime, run_spike_target));
        }

    }

}

void core::reloadSpike(double tnow) {
    // compare membrane potential with threshold voltage for new internal spikes
    for(int i = 0; i<visibleLayer._neurons.size(); i++){
        if(visibleLayer._neurons[i]._memV >= visibleLayer._neurons[i]._Vth && visibleLayer._neurons[i]._active == true){
            std::cout << "<<<<<new spike at visible layer neuron number : " << i << std::endl;
            visibleLayer._neurons[i].turnOFF();
            spike *new_spike = new spike;
            new_spike->_spikeTime = tnow;
            new_spike->_spk.push_back(make_pair(side_v, i));
            new_spike->_reset = true;
            new_spike->_st = true;
            visibleMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
    }

    for(int i = 0; i<hiddenLayer._neurons.size(); i++){
        if(hiddenLayer._neurons[i]._memV >= hiddenLayer._neurons[i]._Vth && hiddenLayer._neurons[i]._active == true){
            std::cout << "<<<<<new spike at hidden layer neuron number : " << i << std::endl;
            hiddenLayer._neurons[i].turnOFF();
            spike *new_spike = new spike;
            new_spike->_spikeTime = tnow;
            new_spike->_spk.push_back(make_pair(side_h, i));
            new_spike->_reset = true;
            new_spike->_st = true;
            hiddenMagazine.push(make_pair(new_spike->_spikeTime, new_spike));
        }
    }
}

void core::eraseTask(int& task_id){

    switch(task_id)
    {
        case 0:
            delete visibleMagazine.top().second;
            visibleMagazine.pop();
            break;
        case 1:
            delete hiddenMagazine.top().second;
            hiddenMagazine.pop();
            break;
        case 2:
            visibleRandomWalkSchedule.pop();
            break;
        case 3:
            hiddenRandomWalkSchedule.pop();
            break;
        default:
            break;
    }
    
}

void core::potentialUpdate(spike& run_spike){
    for(auto it = run_spike._spk.begin(); it != run_spike._spk.end(); it++){
        if(it->first == side_v){  // spike is from side_v so update potential in hidden side
            if(params.enable_gpgm){
                for(int i = 0; i < synapseArray._synapses[it->second].size(); i++){
                    if (hiddenLayer._neurons[i]._active){
                        hiddenLayer._neurons[i]._memV += synapseArray.delta_G(it->second, i);
                    }
                }
            }
        } else {  // spike is from side_h so update potential in visible side
            if(params.enable_gpgm){
                for(int i = 0; i < synapseArray._synapses.size(); i++){
                    if (visibleLayer._neurons[i]._active){
                        visibleLayer._neurons[i]._memV += synapseArray.delta_G(i, it->second);
                    }   
                }
            }
        }
    }
}

void core::STDP(spike& run_spike, int& phase){
    //STDP works only on side_h
    if(phase == data_phase){

    }
}

void core::run_simulation(){
    
    double tend = 0.03;
    double tnow = 0.0;
    double tpre = 0.0;

    spike *run_spike;
    int task;
    int task_id;
    int magazine_side;
    int phase = data_phase;
    long int loop_count = 0;
    cout << setprecision(9);

    generateMagazine(tend); // additional utility if there is no external magazine
    loadMagazine("/Users/gabriel/Development/SNN-TSP/src/build/magazine_injection.json");

    setRandomWalkSchedule(tend, side_v);
    setRandomWalkSchedule(tend, side_h);

    while(1){
        
        task_id = assignTask(&run_spike, tpre, tnow, tend, &magazine_side);
        std::cout << "tpre : " << tpre << " | tnow : " << tnow << std::endl;
        if(task_id == 0 || task_id == 1) { // (_reset, _st) = ( , )
            if (run_spike->_reset) { // (1,-)
                if (run_spike->_st) {                           // (1,1)
                    // For Boltzmann Machine mode, changed hidden spike to visible spike
                    if(run_spike->_spk.begin()->first == side_h && params.enable_BM){
                        run_spike->_spk.begin()->first = side_v;
                    }

                    shootSpike(*run_spike, phase);
                    
                    if (run_spike->_spk.begin()->first == side_h && params.enable_learning && !params.enable_BM){
                        //STDP(*run_spike, phase);
                    }
                    tpre = tnow;
                } else {                                        // (1,0)
                    for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                        if(it->first == side_v){
                            visibleLayer._neurons[it->second].memV_Reset();
                            tpre = tnow;
                        } else {
                            hiddenLayer._neurons[it->second].memV_Reset();
                            tpre = tnow;
                        }
		            }
                }
            } else{ // (0,-)
                if (run_spike->_st) {                           // (0,1)
                    for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                        if(it->first == side_v){
                            visibleLayer._neurons[it->second].turnOFF();
                            tpre = tnow;
                        } else {
                            hiddenLayer._neurons[it->second].turnOFF();
                            tpre = tnow;
                        }
		            }
                } else {                                          // (0,0) but self magazine dummy event
                    if (magazine_side == run_spike->_spk.begin()->first) {
                        for(auto it = run_spike->_spk.begin(); it != run_spike->_spk.end(); it++) {
                            if(it->first == side_v){
                                visibleLayer._neurons[it->second].turnON();
                                tpre = tnow;
                            } else {
                                hiddenLayer._neurons[it->second].turnON();
                                tpre = tnow;
                            }
		                }
                    } else { // (0,0) but spike derived from another side
                        potentialUpdate(*run_spike);
                        reloadSpike(tnow);
                        tpre = tnow;
                    }
                }
            }
        } else if (task_id == 2 || task_id == 3){ // random walk task ids
            reloadSpike(tnow);
            tpre = tnow;
        } else if (task_id == -1){
            std::cout << "No more event!" << std::endl;
            break;
        }
        tpre = tnow;

        eraseTask(task_id);
        loop_count++;
    }

    // After the Loop
    exportSpikeRecorder();
}

// *************EXPORT****************

void core::exportSpikeRecorder() {
    auto result = nlohmann::json{
        {"time", json::array()},
        {"neuron", json::array()},
        };

        for (auto i = 0; i < spikeRecorder.size(); i++) {
            auto& _Gp = result["time"];
            auto& _Gm = result["neuron"];

            _Gp[i].push_back(spikeRecorder[i].first);
            _Gm[i].push_back(spikeRecorder[i].second);
        }

        std::ofstream out("spike_history.json");
        out << result;
}

void core::exportNeuronPotentialToJson(double& tnow) {
    
    for(int side = 0; side < 2; side++) {
        
        std::string filename = potentialFilePath[side];
        
        // Gather neuron potentials
        std::vector<double> neuronPotentials;
        if (side == side_v){
            for (const auto& neuron : visibleLayer._neurons) {
                neuronPotentials.push_back(neuron._memV);
            }
        } else {
            for (const auto& neuron : hiddenLayer._neurons) {
                neuronPotentials.push_back(neuron._memV);
            }
        }

        nlohmann::json existingData;
        // Open the existing JSON file
        std::ifstream inputFile(filename);
        if (inputFile.is_open()) {
            // Load the existing JSON data from the file
            inputFile >> existingData;
            inputFile.close();
        }

        // Create a new JSON object for the neuron data
        json newNeuronData;
        newNeuronData["time"] = tnow;
        newNeuronData["neuronPotentials"] = neuronPotentials;

        // Add the neuron data to the existing JSON object
        existingData["neuronData"].push_back(newNeuronData);

        // Write the modified JSON data back to the file
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cout << "Failed to open the output file." << std::endl;
            return;
        }

        outputFile << std::setw(4) << existingData << std::endl;
        outputFile.close();

    }
}

void core::exportSynapseWeightsToJson(const std::string& filename) {
    // Create a JSON object
    json root;

    // Iterate over the 2D vector and add synapse values to JSON object
    for (const auto& row : synapseArray._synapses) {
        json rowJson;
        for (const auto& synapse : row) {
            json synapseJson;
            synapseJson["gp"] = synapse.Gp;
            synapseJson["gm"] = synapse.Gm;
            rowJson.push_back(synapseJson);
        }
        root.push_back(rowJson);
    }

    // Open the output file
    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        // Write the JSON object to the output file
        outputFile << root.dump(4);
        outputFile.close();
        std::cout << "JSON export successful." << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}