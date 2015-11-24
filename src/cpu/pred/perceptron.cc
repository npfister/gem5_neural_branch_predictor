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
    this->W.resize(size);
    this->size = size;

    // fills W with 0's from [begin, end)
    std::fill(this->W.begin(), this->W.end(), 0);
    // adds another 0 so that we have a total of size 0's
    this->W.push_back(0);
    DPRINTF(Perceptron, "Created PerceptronBP");
}

int8_t getPrediction()
{
    /* TODO - this function may require X as an input, depending on how the history register is handled*/
    return 0; 
}

void
PerceptronBP::reset()
{
    this->W.clear();
    this->W.resize(this->size);

    // fills W with 0's from [begin, end)
    std::fill(this->W.begin(), this->W.end(), 0);
    // adds another 0 so that we have a total of size 0's
    this->W.push_back(0);
    DPRINTF(Perceptron, "Reset PerceptronBP");
}

void
PerceptronBP::train(int8_t result, int8_t y, int8_t theta)
{
    /* TODO */
}
