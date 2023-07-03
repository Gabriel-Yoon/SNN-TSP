#ifndef __TSP_H__
#define __TSP_H__

#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <fstream>  //ofstream
#include <stdio.h>
#include <map>

#include "../utils/np.h"
#include "../chip/synapse.h"

using namespace std;

struct spike_count {
	int num_spike;
};

struct WTA_condition {
	bool route;
	bool iso;
};

enum : int {
	lateral_BM,
	bilateral_BM
};
namespace csp
{
	/* TSP parameter definitions and functions*/
	class tsp {

		friend class core;
		// Neurons
		private: int num_neurons[2];
		private: int num_neurons_bias[2];

		/*---------------------fields-----------------*/
		private: int _numCity;
		private: double _solutionDistance; // 19 for 5 cities, 937 for 26 cities.
		private: std::string _solveMode;
		private: map<double, double> _performance; // (time, performance) format. totalDistance saved in this map
		// city-to-city distance matrix
		private: vector<vector<double>> distance_matrix; // later change it to _distanceMatrix
		private: vector<vector<synapse>> weight_matrix;
		// Weight Setup
		private: double _ExciteSameWTADiffCities;
		private: double _InhibitAdjWTASameCities;
		private: double _InhibitNonAdjWTASameCities;
		private: double _b;
		private: double _grad;

		/*---------------------methods----------------*/
		// csp_tsp Class constructor.
	public: tsp(const char* param_file);

		protected: inline void SetNumNeurons();
		protected: inline void SetWeightMatrix();
		protected: inline void SetWeightMatrixLateralBM();
		// protected: inline void SetWeightMatrixBilateralBM();


	
		// GET
		// public: void GetNumNeurons();

	};
}

#endif