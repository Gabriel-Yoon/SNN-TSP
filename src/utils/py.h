#ifndef _PY_H_
#define _PY_H_

//#define NDEBUG
#include <assert.h>
#include <chrono>
#include <random>
#include <vector>

namespace py {
	inline std::vector<int> range(int start, int stop, int step = 1) {
		std::vector<int> ret;
		for (int i = start; i < stop; i = i + step) {
			ret.push_back(i);
		}
		return ret;
	}

	class rng_1bit {
	public:
		virtual int get_val() = 0;
	};

	class rng_binomial : public rng_1bit {

	public:
		std::mt19937 engine;
		std::bernoulli_distribution rng;

		rng_binomial(int seed, double p) {
			assert(p >= 0 && p <= 0);
			engine = std::mt19937(seed);
			rng = std::bernoulli_distribution(p);
		}

		int get_val() {
			return rng(engine);
			//return rng(engine) & 0x1;
			//return = 0 or 1
		}
	};

	class rng_randrange {

	public:
		std::mt19937 engine;
		std::uniform_int_distribution<int> rng;

		rng_randrange(int seed, int start, int stop) {
			engine = std::mt19937(seed);
			rng = std::uniform_int_distribution<int>(start, stop - 1);
		}

		int get_val() {
			return rng(engine);
			// return = 0 or 1
		}
	};

	class rng_ureal {

	public:
		std::mt19937 engine;
		std::uniform_real_distribution<double> rng;

		rng_ureal(int seed) {
			engine = std::mt19937(seed);
			rng = std::uniform_real_distribution<double>(0.0, 1.0);
		}

		double get_val() {
			return rng(engine);
		}
	};

	class rng_normal {

	public:
		std::mt19937 engine;
		std::normal_distribution<double> rng;

		rng_normal(int seed, double mean, double sigma) {
			engine = std::mt19937(seed);
			rng = std::normal_distribution<double>(mean, sigma);
		}

		double get_val() {
			return rng(engine);
		}
	};

	class rng_exponential {

	public:
		std::mt19937 engine;
		std::exponential_distribution<double> rng;

		rng_exponential(int seed, double mean) {
			assert(mean > 0);
			double lambda = 1 / mean;
			engine = std::mt19937(seed);
			rng = std::exponential_distribution<double>(lambda);
		}

		double get_val() {
			return rng(engine);
		}
	};

} // namespace py


/*
namespace random {

static std::mt19937 generator;
static std::uniform_int_distribution<int> binary_distribution(0, 1); // [a,b]

inline void init() {
	int seed = 1;
	std::mt19937 g1(seed);
	generator = g1;
}

inline bool uniform_binary() { return binary_distribution(generator) == 1; }

inline int randrange(int start, int stop) { // [start, stop)
	assert(start < stop);
	std::uniform_int_distribution<int> distribution(start, stop - 1);
	return distribution(generator);
}

inline double gauss(double mu, double sigma) {
	std::normal_distribution<double> distribution(mu, sigma);
	return distribution(generator);
}
} // namespace random

} // namespace py
*/

#endif /* _PY_H_ */
