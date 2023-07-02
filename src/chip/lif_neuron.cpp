#include "lif_neuron.h"

//**************************************************************************************************************//

//----------------------------------------------------
lif_neuron::lif_neuron(const char* param_file) : neuron(param_file), params(param_file) {
    _leakyTau = params.pt_lk_tau;
}
lif_neuron::~lif_neuron() {

}
//----------------------------------------------------
double& lif_neuron::leakyTau() { // getter function
    return this->_leakyTau;
}
//----------------------------------------------------
void lif_neuron::memV_Leak(double tpre, double tnow) {
    double exp_val = std::exp(-(tnow - tpre) / this->_leakyTau);
    neuron::set_memV(memV() * exp_val);
}
//----------------------------------------------------

//**************************************************************************************************************//