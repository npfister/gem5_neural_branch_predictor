#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/perceptron.hh"
#include "debug/Perceptron.hh"

PerceptronBP::PerceptronBP(uint8_t size)
{
    if (!size) {
      fatal("PerceptronBP must have size > 0");
    }
    this->W = new std::vector<uint8_t>();
    this->W.resize(size);
    DPRINTF(Perceptron, "Created PerceptronBP");
}

int8_t getPrediction()
{
    /* TODO */
    return 0; 
}

void
PerceptronBP::reset()
{
    this->W.clear();
    this->W.resize(this->size);
    DPRINTF(Perceptron, "Reset PerceptronBP");
}

void
PerceptronBP::train(int8_t result, int8_t y, int8_t theta)
{
    /* TODO */
}
