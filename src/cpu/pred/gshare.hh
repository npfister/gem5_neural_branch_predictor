/*
 * Authors: John Skubic
 */

#ifndef __CPU_O3_GSHARE_PRED_HH__
#define __CPU_O3_GSHARE_PRED_HH__

#include <vector>

#include "base/types.hh"
#include "cpu/o3/sat_counter.hh"

/**
 * Implements a global predictor that uses the PC to index into a table of
 * counters.  Note that any time a pointer to the bp_history is given, it
 * should be NULL using this predictor because it does not have any branch
 * predictor state that needs to be recorded or updated; the update can be
 * determined solely by the branch being taken or not taken.
 */
class GshareBP
{
  public:
    /**
     * Default branch predictor constructor.
     * @param globalPredictorSize Size of the global predictor.
     * @param globalCtrBits Number of bits per counter.
     * @param instShiftAmt Offset amount for instructions to ignore alignment.
     */
    GshareBP(unsigned globalPredictorSize, unsigned globalCtrBits, unsigned globalHistoryLen);

    /**
     * Looks up the given address in the branch predictor and returns
     * a true/false value as to whether it is taken.
     * @param branch_addr The address of the branch to look up.
     * @param bp_history Pointer to any bp history state.
     * @return Whether or not the branch is taken.
     */
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

    void uncondBr(void * &bp_history);
  private:
    /**
     *  Returns the taken/not taken prediction given the value of the
     *  counter.
     *  @param count The value of the counter.
     *  @return The prediction based on the counter value.
     */
    inline bool getPrediction(uint8_t &count);

    /** Calculates the global index based on the PC. */
    inline unsigned getGlobalIndex(Addr &PC, unsigned history);

    /** Array of counters that make up the global predictor. */
    std::vector<SatCounter> globalCtrs;

    /** Size of the global predictor. */
    unsigned globalPredictorSize;

    /** Number of sets. */
    unsigned globalPredictorSets;

    /** Number of bits of the global predictor's counters. */
    unsigned globalCtrBits;

    /** Mask to get the proper global history. */
    unsigned globalHistoryMask;

    /** Global history register. */
    unsigned globalHistory;    

    unsigned globalHistoryLen;

    unsigned indexMask;
   
    struct BPHistory {
	    unsigned globalHistory;
	  };

};

#endif // __CPU_O3_GSHARE_PRED_HH__
