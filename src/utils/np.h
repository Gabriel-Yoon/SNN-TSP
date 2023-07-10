
#ifndef _NP_H_
#define _NP_H_

//#define NDEBUG
#include <assert.h>

#include <tuple>
#include <vector>

namespace np {
// public
template <typename T>
using array_1d = std::vector<T>;
template <typename T>
using array_2d = std::vector<std::vector<T>>;

// public
inline array_1d<int> arange(uint32_t stop) {
    array_1d<int> ret;
    for (uint32_t i = 0; i < stop; i++) {
        ret.push_back(i);
    }
    return ret;
}

// private
template <typename T>
array_2d<T> __make_array_2d(uint32_t rows, uint32_t cols, T initial_value) {
    array_2d<T> ret;
    ret.insert(ret.begin(), rows, array_1d<T>{});
    for (uint32_t i = 0; i < rows; i++) {
        ret[i].insert(ret[i].begin(), cols, initial_value);
    }
    return ret;
}

// private
template <typename T>
array_1d<T> __make_array_1d(uint32_t length, T initial_value) {
    array_1d<T> ret;
    ret.insert(ret.begin(), length, initial_value);
    return ret;
}

// public
template <typename T = double>
array_2d<T> zeros(uint32_t rows, uint32_t cols) {
    return __make_array_2d<T>(rows, cols, 0);
}

// public
template <typename T = double>
array_1d<T> zeros(uint32_t length) {
    return __make_array_1d<T>(length, 0);
}

// public
template <typename T = double>
array_1d<T> ones(uint32_t length) {
    return __make_array_1d<T>(length, 1);
}

// public
template <typename T = double>
array_2d<T> tile(array_2d<T> &A, std::tuple<uint32_t, uint32_t> reps) {
    const auto rows = A.size();
    const auto cols = A[0].size();
    const auto [mul_r, mul_c] = reps;

    array_2d<T> ret;
    ret.insert(ret.begin(), rows * mul_r, array_1d<T>{});
    uint32_t row_cur = 0;
    for (uint32_t i = 0; i < rows * mul_r; i++) {
        ret[i].reserve(cols * mul_c);
        for (uint32_t j = 0; j < mul_c; j++) {
            ret[i].insert(ret[i].begin(), A[row_cur].begin(), A[row_cur].end());
        }
        row_cur++;
        if (row_cur == rows) {
            row_cur = 0;
        }
    }

    return ret;
}


// public
template <typename T = double>
unsigned int argmax(array_1d<T> &A) {
    unsigned int ret = 0;
    T best = A[0];
    for (unsigned int i = 1; i < A.size(); i++) {
        if (A[i] > best) {
            best = A[i];
            ret = i;
        }
    }
    return ret;
}
} // namespace np

/*
namespace random {
static std::mt19937 generator;

inline void init() {
    int seed = 2;
    std::mt19937 g1(seed);
    generator = g1;
}

inline int binomial(double p) {
    assert(n > 0);
    assert(p > 0);
    std::bernoulli_distribution distribution(p);
    return distribution(generator);
}

inline double exponential(double mean) {
    assert(mean > 0);
    double lambda = 1 / mean;
    std::exponential_distribution<double> distribution(lambda);
    return distribution(generator);
}
} // namespace random
} // namespace np
*/
#endif /* _NP_H_ */
