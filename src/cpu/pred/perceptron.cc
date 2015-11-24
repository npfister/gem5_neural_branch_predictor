#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/perceptron.hh"
#include "debug/Perceptron.hh"
#include <numeric>

PerceptronBP::PerceptronBP(uint8_t size)
{
    if (!size) {
      fatal("PerceptronBP must have size > 0");
    }
    this->W.resize(size);
    this->size = size;

    // fills W with 0's from [begin, end)
    std::fill(this->W.begin(), this->W.end(), 0);
    DPRINTF(Perceptron, "Created PerceptronBP");
}

int8_t
PerceptronBP::getPrediction(std::vector<int8_t>& X)
{
    /* TODO - this function may require X as an input, depending on how the history register is handled*/
    return std::inner_product(X.begin(), X.end(), this->W.begin(), 0); 
}

void
PerceptronBP::reset()
{
    this->W.clear();
    this->W.resize(this->size);

    // fills W with 0's from [begin, end)
    std::fill(this->W.begin(), this->W.end(), 0);
    DPRINTF(Perceptron, "Reset PerceptronBP");
}


void 
PerceptronBP::train(int8_t branch_outcome, int8_t perceptron_output, int8_t training_threshold, std::vector<int8_t> &X)
{
    if ( ((perceptron_output < 0) ? -1 : 1 ) != branch_outcome || abs(perceptron_output)<=training_threshold) {//incorrect perceptron prediction. Upgrade the perceptron predictor
        for(int i=0; i< this->W.size(); i++) {
            W[i]= W[i]+ ((perceptron_output < 0) ? -1 : 1) * X[i]; //Increase or decrease weight vectors
        }
    }
}