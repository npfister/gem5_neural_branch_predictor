#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/perceptron.hh"
#include "debug/Perceptron.hh"
#include <numeric>
#include <string>

PerceptronBP::PerceptronBP(uint32_t size, uint32_t theta)
{
    if (!size) {
      fatal("PerceptronBP must have size > 0");
    }
    this->W.resize(size);
    this->size = size;
    this->theta = theta;

    // fills W with 0's from [begin, end)
    std::fill(this->W.begin(), this->W.end(), 0);
}

int32_t
PerceptronBP::getPrediction(std::vector<int8_t>& X)
{
    /* TODO - this function may require X as an input, depending on how the history register is handled*/
    int32_t innerProdManual = 0;
    int32_t innerProdStd =  std::inner_product(X.begin(), X.end(), this->W.begin(), 0);

    for(int i=0;i < X.size();i++){
      innerProdManual += X[i] * this->W[i];
    }

    assert(innerProdStd == innerProdManual);
    assert(X.size() == this->W.size());
    return innerProdStd;
  
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
PerceptronBP::train(int8_t branch_outcome, int32_t perceptron_output, int32_t training_threshold, std::vector<int8_t> &X)
{
    std::string s = "W: ";
    DPRINTF(Perceptron, "Perceptron train entered\n");
    if (this->changeToPlusMinusOne(perceptron_output) != branch_outcome || abs(perceptron_output)<=training_threshold) {//incorrect perceptron prediction. Upgrade the perceptron predictor
        for(int i=0; i< this->W.size(); i++) {
            W[i]= W[i]+ branch_outcome * X[i]; //Increase or decrease weight vectors
            if(abs(W[i]) > this->theta){
              if (W[i] < 0){
                W[i] = theta * -1;
              }
              else{
                W[i] = theta;
              }
            }
            s.append(std::to_string((long long int)W[i]));
            s.append(", ");
        }
    }
    DPRINTF(Perceptron, "%s\n", s);
}

inline int8_t
PerceptronBP::changeToPlusMinusOne(int32_t input)
{
  return (input >= 0) ? 1 : -1;
}
