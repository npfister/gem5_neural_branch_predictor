/*
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: John Skubic
 */

#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "cpu/pred/gshare.hh"
#include "debug/Fetch.hh"

GshareBP::GshareBP(unsigned _globalPredictorSize,
                  unsigned _globalCtrBits,
                  unsigned _globalHistoryLen)
    : globalPredictorSize(_globalPredictorSize),
      globalCtrBits(_globalCtrBits),
      globalHistoryLen(_globalHistoryLen)
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
    globalHistoryMask = 0;
    for(int i=0;i<globalHistoryLen;i++){
      globalHistoryMask = (globalHistoryMask << 1) | 1;
    }

    indexMask = globalPredictorSets-1;


    // Setup the array of counters for the global predictor.
    globalCtrs.resize(globalPredictorSets);

    for (unsigned i = 0; i < globalPredictorSets; ++i)
        globalCtrs[i].setBits(_globalCtrBits);

    DPRINTF(Fetch, "Branch predictor: ghsare predictor size: %i\n",
            globalPredictorSize);

    DPRINTF(Fetch, "Branch predictor: gshare counter bits: %i\n", globalCtrBits);

    DPRINTF(Fetch, "Branch predictor: gshare history bits: %i\n", globalHistoryLen);

    DPRINTF(Fetch, "Branch predictor: gshare num sets: %d\n",globalPredictorSets);

    DPRINTF(Fetch, "Branch predictor: gshare index mask: %#x\n", indexMask);

    DPRINTF(Fetch, "Branch predictor: history mask: %#x\n", globalHistoryMask);
}

void
GshareBP::reset()
{
    for (unsigned i = 0; i < globalPredictorSets; ++i) {
        globalCtrs[i].reset();
    }
}

void
GshareBP::BTBUpdate(Addr &branch_addr, void * &bp_history)
{
// Called to update predictor history when
// a BTB entry is invalid or not found.
  //update global history to not Taken
  //globalHistory = globalHistory & (globalHistoryMask - 1);
}


bool
GshareBP::lookup(Addr &branch_addr, void * &bp_history)
{
    bool taken;
    uint8_t counter_val;
    //idx is xor of branch addr and globalHistory
    unsigned global_predictor_idx = getGlobalIndex(branch_addr, globalHistory);
    
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory;
    bp_history = static_cast<void *>(history);

    DPRINTF(Fetch, "IDX: %d SETS: %d\n",global_predictor_idx, this->globalPredictorSets);
    assert (global_predictor_idx < this->globalPredictorSets);

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
GshareBP::update(Addr &branch_addr, bool taken, void *bp_history)
{
    unsigned global_predictor_idx;
    BPHistory *history;

    // Update the global predictor.
    if (bp_history){
      history = static_cast<BPHistory *>(bp_history);

      global_predictor_idx = getGlobalIndex(branch_addr, history->globalHistory);

      DPRINTF(Fetch, "IDX: %d SETS: %d\n",global_predictor_idx, this->globalPredictorSets);
      assert (global_predictor_idx < this->globalPredictorSets);

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
      delete history;
    }
}

inline
bool
GshareBP::getPrediction(uint8_t &count)
{
    // Get the MSB of the count
    return (count >> (globalCtrBits - 1));
}

inline
unsigned
GshareBP::getGlobalIndex(Addr &branch_addr, unsigned history)
{
    return ((branch_addr ^ (history & globalHistoryMask)) & indexMask);
}

void
GshareBP::uncondBr(void * &bp_history)
{
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory;
    bp_history = static_cast<void *>(history);
}

void 
GshareBP::squash(void *bp_history)
{
    BPHistory *history = static_cast<BPHistory *>(bp_history);
    delete history;
}
