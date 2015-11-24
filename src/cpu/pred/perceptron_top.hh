/*
* Authors: Alex Ionescu, Nick Pfister
*/

#ifndef __CPU_O3_PERCEPTRON_TOP_PRED_HH__
#define __CPU_O3_PERCEPTRON_TOP_PRED_HH__

#include <vector>

#include "base/types.hh"
#include "cpu/o3/sat_counter.hh"
#include "cpu/pred/perceptron.hh"

/**
 * Implements a global predictor that uses the PC to index into a table of
 * counters.  Note that any time a pointer to the bp_history is given, it
 * should be NULL using this predictor because it does not have any branch
 * predictor state that needs to be recorded or updated; the update can be
 * determined solely by the branch being taken or not taken.
 */
class PerceptronBP_Top
{
  public:
    /**
     * Default branch predictor constructor.
     * @param globalPredictorSize number of perceptrons.
     * @param globalHistBits Number of bits in global history register.
     */
    PerceptronBP_Top(unsigned globalPredictorSize, unsigned globalHistBits, int8_t theta);

    /**
     * Looks up the given address in the branch predictor and returns
     * a true/false value as to whether it is taken.
     * @param branch_addr The address of the branch to look up.
     * @param bp_history Pointer to any bp history state.
     * @return Whether or not the branch is taken.
     */                             //NULL
    bool lookup(Addr &branch_addr, void * &bp_history);

    /**
     * Updates the branch predictor to Not Taken if a BTB entry is
     * invalid or not found.
     * @param branch_addr The address of the branch to look up.
     * @param bp_history Pointer to any bp history state.
     * @return Whether or not the branch is taken.
     */
    void BTBUpdate(Addr &branch_addr, void * &bp_history);

    /**
     * Updates the branch predictor with the actual result of a branch.
     * @param branch_addr The address of the branch to update.
     * @param taken Whether or not the branch was taken.
     */
    void update(Addr &branch_addr, bool taken, void *bp_history);

    void squash(void *bp_history);

    void reset();

  private:

    /** Calculates the global index based on the PC. */
    inline unsigned getGlobalIndex(Addr &PC);

    /** Array of counters that make up the global predictor. */
    std::vector<PerceptronBP*> perceptronTable;

    /** Size of the global predictor. */
    unsigned globalPredictorSize;

    /** Number of bits of the global predictor's counters. */
    unsigned globalHistBits;

    /** Mask to get the proper global history. */
    unsigned globalHistoryMask;

    /** Global history register. */
    std::vector<int8_t> X;

    /** Training constraint */
    int8_t theta;

    struct BPHistory {
        int8_t perceptron_y;
	};

};

#endif
