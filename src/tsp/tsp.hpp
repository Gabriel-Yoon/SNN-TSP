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

using namespace std;

struct spike_count {
	int num_spike;
};

struct WTA_condition {
	bool route;
	bool iso;
};

enum : int {
	side_v,
	side_h
};

enum : int {
	lateral_BM,
	bilateral_BM
};
namespace csp
{
	/* TSP parameter definitions and functions*/
	class tsp {

		// Neurons
		private: int num_neurons[2];
		private: int num_neurons_bias[2];

		/*---------------------fields-----------------*/
		private: int _numCity;
		private: double _solutionDistance; // 19 for 5 cities, 937 for 26 cities.
		private: std::string _solveMode;
		private: map<double, double> _performance; // (time, performance) format. totalDistance saved in this map
		// city-to-city distance matrix
		private: vector<vector<double>> _distanceMatrix;
		// Weight Setup
		private: double _sameWTADiffCities;
		private: double _adjWTASameCities;
		private: double _nonAdjWTASameCities;
		private: double _b;
		private: double _grad;
		// WTA condition fulfillment parameters
		vector<vector<struct WTA_condition>> WTA; // traveling order marking array
		vector<vector<struct spike_count>> spike_counter; // spike counting array
		vector<double> last_spike;
		vector<bool> WTA_at_least_one_spike; // instantaneous firing probability checking array

		/*---------------------methods----------------*/
		// csp_tsp Class constructor.
		public: tsp(const char* param_file);

		protected: inline void SetNumNeurons();
		protected: inline void SetWeightMatrix();
		protected: inline void SetWeightMatrixLateralBM();
		protected: inline void SetWeightMatrixBilateralBM();

		// CSP function
		public: void wta_initialize(double* potential[], double init_val);
		public: void wta_condition_update(sm_spk& spk_now, double tnow, double tend);
		public: void cap_iso_update();
	
		// GET
		// public: void GetNumNeurons();

		// Export method
		public: void export_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tnow, double tend);
		public: void export_num_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend);
		public: void export_wta_spike_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend);
		public: void export_potential_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend);
		public: void export_travel_info_to_csv(tsp& tsp, ofstream& exportFile, sm_spk& spk_now, double tend);
		public: void export_performance_info_to_csv(tsp& tsp, ofstream& exportFile, double tnow);

	};
}

#endif