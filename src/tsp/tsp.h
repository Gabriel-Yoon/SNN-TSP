#ifndef __TSP_H__
#define __TSP_H__

#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <fstream>  //ofstream
#include <stdio.h>
#include <map>

#include "../utils/utils.h"
#include "../utils/np.h"

using namespace std;

struct cell {
	double Gp;
	double Gm;
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
		public: int num_neurons[2];
		public: int num_neurons_bias[2];

		/*---------------------fields-----------------*/
		public: int _numCity;
		public: std::vector<int> _itinerary;
		public: double _solutionDistance; // 19 for 5 cities, 937 for 26 cities.
		private: std::string _solveMode;
		private: map<double, double> _performance; // (time, performance) format. totalDistance saved in this map
		// city-to-city distance matrix
		private: vector<vector<double>> distance_matrix; // later change it to _distanceMatrix
		private: vector<vector<cell>> weight_matrix;
		// Weight Setup
		private: double _ExciteSameWTADiffCities;
		private: double _InhibitAdjWTASameCities;
		private: double _InhibitNonAdjWTASameCities;
		private: double _b;
		private: double _grad;

		/*---------------------methods----------------*/
		// csp_tsp Class constructor.
		public: tsp(const std::string& tsp_json_file_path, const std::string& tsp_matrix_file_path, const std::string& tsp_itinerary_file_path);
		public: void initialize();

		protected: inline void SetNumNeurons();
		protected: inline void SetWeightMatrix();
		protected: inline void SetWeightMatrixLateralBM();
		// protected: inline void SetWeightMatrixBilateralBM();

		public: void exportWeightToFile(std::string filename);
		
		public: void read_and_write_json_file(const std::string& file_path);
		public: void read_distance_matrix(const std::string& file_path);
		public: void load_itinerary(const std::string& file_path);
		public: void calculate_minimal_total_distance();
		public: void copy_json_file(const std::string& source_file_path, const std::string& destination_file_path);
		// GET
		// public: void GetNumNeurons();

	};
}

#endif