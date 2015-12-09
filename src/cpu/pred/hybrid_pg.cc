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
#include "cpu/pred/hybrid_pg.hh"
#include "debug/Fetch.hh"
#include "cpu/pred/perceptron.hh"


HybridpgBP::HybridpgBP(unsigned _globalPredictorSize,
                  unsigned _globalHistoryLen,
                  int32_t _theta)
    : globalPredictorSize(_globalPredictorSize),
      globalHistoryLen(_globalHistoryLen),
      theta(_theta)
{
    if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid global predictor size!\n");
    }

    globalPredictorSets = floorPow2(globalPredictorSize / (_globalHistoryLen * ceilLog2(theta)));

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

	  this->X.push_back(1);
	  for(int i=1;i < _globalHistoryLen; i++) { //
		  this->X.push_back(-1);
	  }

    // Setup the array of counters for the global predictor.
    for (unsigned i = 0; i < globalPredictorSets; ++i)
      this->perceptronTable.push_back(new PerceptronBP(_globalHistoryLen, theta));

    theta = _theta;
	   
    DPRINTF(Fetch, "Hybrid Predictor:\nSize: %d\nHistoryLen: %d\nHistoryMask %x\nIdxMask %x\nglobalPredictorSets: %d\ntheta %d\n\n", globalPredictorSize, globalHistoryLen, globalHistoryMask, indexMask, globalPredictorSets, theta);
}

void
HybridpgBP::reset()
{

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
    unsigned global_predictor_idx;

    //idx is xor of branch addr and globalHistory
    global_predictor_idx = getGlobalIndex(branch_addr, globalHistory);

    DPRINTF(Fetch, "LOOKUP: idx %x addr %x history %x\n", global_predictor_idx, branch_addr, globalHistory);

	  PerceptronBP* curr_perceptron = this->perceptronTable[global_predictor_idx];
	  BPHistory *history = new BPHistory;
	  history->perceptron_y = curr_perceptron->getPrediction(this->X);
    history->globalHistory = globalHistory;
    history->X = X;
	  bp_history = static_cast<void *>(history);
    taken = (history->perceptron_y) >= 0;
    return taken;
}

void
HybridpgBP::update(Addr &branch_addr, bool taken, void *bp_history)
{
    unsigned global_predictor_idx;
    BPHistory *history;
    // Update the global predictor.
    
    DPRINTF(Fetch, "Entering Hybrid Update\n");

  if (bp_history){

    //train the same perceptron that made the prediction
    history = static_cast<BPHistory *>(bp_history);
    global_predictor_idx = getGlobalIndex(branch_addr,history->globalHistory);

    DPRINTF(Fetch, "UPDATE: idx %x addr %x history %x\n", global_predictor_idx, branch_addr, history->globalHistory);
 
    PerceptronBP* curr_perceptron = this->perceptronTable[global_predictor_idx];
    curr_perceptron->train(this->changeToPlusMinusOne((int32_t)taken), history->perceptron_y, this->theta, history->X);
    this->X.insert(this->X.begin() + 1, this->changeToPlusMinusOne((int32_t)taken));
    this->X.pop_back();

    if(taken)
      globalHistory = (globalHistory << 1) | 1;
    else
      globalHistory = globalHistory << 1;

    globalHistory = globalHistory & globalHistoryMask;
    delete history;
  }
}

inline
unsigned
HybridpgBP::getGlobalIndex(Addr &branch_addr, unsigned history)
{
    return ((branch_addr ^ (history & globalHistoryMask)) & indexMask);
}

void 
HybridpgBP::uncondBr(void * &bp_history)
{
    BPHistory *history = new BPHistory;
    history->perceptron_y = 1; //anything greater than 0 is taken
    history->globalHistory = globalHistory;
    history->X = X;
   	bp_history = static_cast<void *>(history);
}

inline int8_t
HybridpgBP::changeToPlusMinusOne(int32_t input)
{
  return (input > 0) ? 1 : -1;
}

void
HybridpgBP::squash(void *bp_history)
{
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    // Delete this BPHistory now that we're done with it.
    delete history;
}
