#include <math.h>
#include <algorithm>
#include <unistd.h>

#include "core.h"
#include "spk.h"

core::core(const char* param_file) : params(param_file)
{
	_visibleNumNeurons;
	_hiddenNumNeurons;
}

void core::weight_load(int cell_type, std::string fweight) {
	std::ifstream is;
	is.open(fweight, std::ios::binary);
	if (is.fail()) {
		std::cout << "Error opening file " << fweight << ". Exit." << std::endl;
		exit(1);
	}

	//Check file size
	is.seekg(0, std::ios::end);
	auto eofpos = is.tellg();
	is.clear();

	is.seekg(0, std::ios::beg);
	auto begpos = is.tellg();
	long int fsize = eofpos - begpos;
	long int dsize_wt = sizeof(double) * _visibleNumNeurons * _hiddenNumNeurons;
	if ((fsize != dsize_wt)) {
		std::clog << "Unexpected file size " << fsize << ". Exit." << std::endl;
		exit(1);
	}

	double weight = 0.0;

	for (int i = 0; i < _visibleNumNeurons; i++) {
		for (int j = 0; j < _hiddenNumNeurons; j++) {
			is.read((char*)&weight, sizeof(double));
			if (cell_type == wij_gp) {
				if (weight > params.max_weight) {
					weight = params.max_weight;
				}
				else if (weight < params.min_weight) {
					weight = params.min_weight;
				}
				synapses[i][j].gp = weight;
			}
			else {
				if (weight > params.max_weight) {
					weight = params.max_weight;
				}
				else if (weight < params.min_weight) {
					weight = params.min_weight;
				}
				synapses[i][j].gm = weight;
			}
		}
	}
	is.close();
}

void core::weight_save(int cell_type, std::string filename) {
	std::ofstream os;
	os.open(filename, std::ios::binary);

	for (int i = 0; i < _visibleNumNeurons; i++) {
		for (int j = 0; j < _hiddenNumNeurons; j++) {
			if (cell_type == wij_gp) {
				os.write((char*)&(synapses[i][j].gp), sizeof(double));
			}
			else {
				os.write((char*)&(synapses[i][j].gm), sizeof(double));
			}
		}
	}
	os.close();
}