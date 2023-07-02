#include <vector>
#include <string>
#include <cstdint>
#include <utility>

enum side {
    side_v,
    side_h
};

extern char side_chr[2];

// Synapse 특성을 새로 도입할 경우 바꿀 수 있는 인자들을 모아서 바꿔보자.

class synapse {

    friend class core;
    friend class spk;
    /*---------------------fields-----------------*/

public:
    synapse(uint32_t row, uint32_t col) {
        // row : neuron num of visible layer
        // col : neuron num of hidden layer
        position = std::make_pair(row, col);
        // synapse value
        gp = 0;
        gm = 0;
    }

private:
    std::pair<uint32_t, uint32_t> position;

public:
    double gp;
    double gm;
};
