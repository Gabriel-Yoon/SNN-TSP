
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "fs.h"
#include "np.h"
#include "py.h"

#include "neuron.h"
#include "synapse.h"
#include "param.h"


struct real_data {
	double delta_g;
	double standard_g;
};

struct spk_cmp {
	bool operator()(std::pair<double, spk> a, std::pair<double, spk> b) {
		if (a.first == b.first) return a.second.position.size() < b.second.position.size();
		return a.first > b.first;
	}
};

//**************************************************************************************************************//
class core
{
	private: param params;
	std::ofstream os;
	/*---------------------fields-----------------*/
	private: int _visibleNumNeurons;
	private: int _hiddenNumNeurons;

	std::vector<struct real_data> g_potentiation;
	std::vector<struct real_data> g_depression;
	std::vector<double> g_differ;
	std::vector<std::vector<synapse>> synapses;
	std::vector<std::vector<neuron>> layers;

	int Gp_set = 0;
	int Gp_reset = 0;
	int Gm_set = 0;
	int Gm_reset = 0;

public:
	core(const char* param_file);
	double run(int epoch);
	void print_params();
	void initialize(char* fwij_gp, char* fwij_gm, char* data_pot, char* data_dep);

private:
	template <typename T> void gen_spike(const std::string type, const std::vector<int>& N_set, const std::string location, const std::vector<T>& data, const double end_point,
		const double start_point, std::priority_queue<spk, std::vector<spk>, decltype(spk_comparator)>& spk_queue, std::priority_queue<spk, std::vector<spk>, decltype(spk_comparator)>& spk_wup_queue);

	void emplace_spike(const std::string type, const int v_or_h, const int num, double t_now, const bool is_neuron_delay,
		const bool is_synaptic_delay, std::priority_queue<spk, std::vector<spk>, decltype(spk_comparator)>& spk_queue_pri, bool is_fire);

	void weight_load(int cell_type, std::string fweight);
	void weight_save(int cell_type, std::string filename);

