/*
 * Authors: John Skubic
 */

#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/hybrid_pg.hh"
#include "debug/Fetch.hh"

HybridpgBP::HybridpgBP(unsigned _globalPredictorSize,
                 unsigned _globalCtrBits)
    : globalPredictorSize(_globalPredictorSize),
      globalCtrBits(_globalCtrBits)
{
    if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid global predictor size!\n");
    }

    globalPredictorSets = globalPredictorSize / globalCtrBits;

    if (!isPowerOf2(globalPredictorSets)) {
        fatal("Invalid number of global predictor sets! Check globalCtrBits.\n");
    }

    //clear the global history
    globalHistory = 0;
  
    //set up the global history mask
    globalHistoryMask = (1 << globalPredictorSets) - 1;

    DPRINTF(Fetch, "Branch predictor: history mask: %#x\n", globalHistoryMask);

    // Setup the array of counters for the global predictor.
    globalCtrs.resize(globalPredictorSets);

    for (unsigned i = 0; i < globalPredictorSets; ++i)
        globalCtrs[i].setBits(_globalCtrBits);

    DPRINTF(Fetch, "Branch predictor: ghsare predictor size: %i\n",
            globalPredictorSize);

    DPRINTF(Fetch, "Branch predictor: gshare counter bits: %i\n", globalCtrBits);

}

void
HybridpgBP::reset()
{
    for (unsigned i = 0; i < globalPredictorSets; ++i) {
        globalCtrs[i].reset();
    }
}

void
HybridpgBP::BTBUpdate(Addr &branch_addr, void * &bp_history)
{
// Called to update predictor history when
// a BTB entry is invalid or not found.
  //update global history to not Taken
  //globalHistory = globalHistory & (globalHistoryMask - 1);
}


bool
HybridpgBP::lookup(Addr &branch_addr, void * &bp_history)
{
    bool taken;
    uint8_t counter_val;
    //idx is xor of branch addr and globalHistory
    unsigned global_predictor_idx = getGlobalIndex(branch_addr);

    DPRINTF(Fetch, "Branch predictor: Looking up index %#x\n",
            global_predictor_idx);

    counter_val = globalCtrs[global_predictor_idx].read();

    DPRINTF(Fetch, "Branch predictor: prediction is %i.\n",
            (int)counter_val);

    taken = getPrediction(counter_val);

#if 0
    // Speculative update.
    if (taken) {
        DPRINTF(Fetch, "Branch predictor: Branch updated as taken.\n");
        globalCtrs[global_predictor_idx].increment();
    } else {
        DPRINTF(Fetch, "Branch predictor: Branch updated as not taken.\n");
        globalCtrs[global_predictor_idx].decrement();
    }
#endif

    return taken;
}

void
HybridpgBP::update(Addr &branch_addr, bool taken, void *bp_history)
{
    assert(bp_history == NULL);
    unsigned global_predictor_idx;

    // Update the global predictor.
    global_predictor_idx = getGlobalIndex(branch_addr);

    DPRINTF(Fetch, "Branch predictor: Looking up index %#x\n",
            global_predictor_idx);

    if (taken) {
        DPRINTF(Fetch, "Branch predictor: Branch updated as taken.\n");
        globalCtrs[global_predictor_idx].increment();
    } else {
        DPRINTF(Fetch, "Branch predictor: Branch updated as not taken.\n");
        globalCtrs[global_predictor_idx].decrement();
    }

    //update global history
    if(taken)
      globalHistory = (globalHistory << 1) | 1;
    else
      globalHistory = globalHistory << 1;

    globalHistory = globalHistory & globalHistoryMask;
}

inline
bool
HybridpgBP::getPrediction(uint8_t &count)
{
    // Get the MSB of the count
    return (count >> (globalCtrBits - 1));
}

inline
unsigned
HybridpgBP::getGlobalIndex(Addr &branch_addr)
{
    return ((branch_addr ^ globalHistory) & globalHistoryMask);
}
