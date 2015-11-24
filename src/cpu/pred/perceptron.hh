#ifndef __CPU_O3_PERCEPTRON_LOCAL_PRED_HH__
#define __CPU_O3_PERCEPTRON_LOCAL_PRED_HH__

#include <vector>

#include "base/types.hh"

/**
 * Implements a local predictor that uses the PC to index into a table of
 * counters.  Note that any time a pointer to the bp_history is given, it
 * should be NULL using this predictor because it does not have any branch
 * predictor state that needs to be recorded or updated; the update can be
 * determined solely by the branch being taken or not taken.
 */
class PerceptronBP
{
  public:
    /**
     * Default branch predictor constructor.
     * @param size How many elements the W vector should be. Must be >= 1
     */
    PerceptronBP(uint8_t size);

    /**
     * Computes the dot product of X and W
     * @return A number > 0 implies predict taken
     */
    int8_t getPrediction(std::vector<int8_t>& X); 

    /*
     * Resets the perceptrion's W values
     */
    void reset();

    /**
     * Trains the perceptron branch predictor
     * @param branch_outcome actual result of last branch - Taken = 1, not taken = -1
     * @param perceptron_output predicted result of branch - Taken = 1, note taken = -1 
     * @param training_threshold training threshold
     * @param X - vector for global branch history
     */
    void train(int8_t branch_outcome, int8_t perceptron_output, int8_t training_threshold, std::vector<int8_t>& X);
  private:
    /** W array which stores weights for perceptrion branch predictor */
    uint8_t size;
    std::vector<int8_t> W;
};

#endif // __CPU_O3_PERCEPTRON_LOCAL_PRED_HH__
