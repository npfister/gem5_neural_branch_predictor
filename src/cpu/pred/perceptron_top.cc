/*
* Authors: Alex Ionescu, Nick Pfister
*/

#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "debug/Perceptron.hh"
#include "cpu/pred/perceptron.hh"
#include "cpu/pred/perceptron_top.hh"

PerceptronBP_Top::PerceptronBP_Top(unsigned globalPredictorSize, unsigned globalHistBits, int32_t theta)
{

  DPRINTF(Perceptron, "BP_Top Constructor Start %d %d %d\n", globalPredictorSize, globalHistBits, theta);
	this->globalPredictorSize = globalPredictorSize;
	this->globalHistBits = globalHistBits;

	if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid perceptron table size!\n");
    }


	for(int i=0;i < globalPredictorSize; i++) { //create perceprton table
		this->perceptronTable.push_back(new PerceptronBP(globalHistBits, theta));
	}

	this->X.push_back(1);
	for(int i=1;i < globalHistBits; i++) { //
		this->X.push_back(-1);
	}

	this->globalHistoryMask = (unsigned)(power(2,globalHistBits) - 1);

	this->theta = theta;
  DPRINTF(Perceptron, "BP_Top Constructed %d %d %d %d\n", this->globalPredictorSize, this->globalHistBits, this->theta, this->globalHistoryMask);

  this->missCount = 0;
}

bool
PerceptronBP_Top::lookup(Addr &branch_addr, void * &bp_history)
{
  DPRINTF(Perceptron, "BP_Top entered lookup\n");
	//PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & this->globalHistoryMask];
	PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & (this->globalPredictorSize - 1)];
	BPHistory *history = new BPHistory;
	history->perceptron_y = curr_perceptron->getPrediction(this->X);
	bp_history = static_cast<void *>(history);

	// y 
  DPRINTF(Perceptron, "BP_Top lookup y: %d\n", history->perceptron_y);
  DPRINTF(Perceptron, "BP_Top branch_addr %x\n", branch_addr);
	return (history->perceptron_y) > 0;

}

void
PerceptronBP_Top::BTBUpdate(Addr &branch_addr, void * &bp_history)
{

}

void
PerceptronBP_Top::update(Addr &branch_addr, bool taken, void *bp_history)
{

  DPRINTF(Perceptron, "BP_Top entered update, yhist %d\n",  static_cast<BPHistory *>(bp_history)->perceptron_y);

  if (bp_history){
    //PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & this->globalHistoryMask];
    PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & (this->globalPredictorSize - 1)];
    curr_perceptron->train(this->changeToPlusMinusOne((int32_t)taken), static_cast<BPHistory *>(bp_history)->perceptron_y, this->theta, this->X);
    this->X.insert(this->X.begin() + 1, this->changeToPlusMinusOne((int32_t)taken));
    this->X.pop_back();
    DPRINTF(Perceptron, "BP_Top update after train %d\n", curr_perceptron->getPrediction(this->X)); //static_cast<BPHistory *>(bp_history)->perceptron_y);
    DPRINTF(Perceptron, "BP_Top update taken %d\n", taken);
    DPRINTF(Perceptron, "BP_Top update branch_addr %x\n", branch_addr);

    if (taken != (static_cast<BPHistory *>(bp_history)->perceptron_y > 0)){
      this->missCount++;
      DPRINTF(Perceptron, "Miss Count: %d\n", this->missCount);
    }

    delete static_cast<BPHistory *>(bp_history);
  }

}

void
PerceptronBP_Top::squash(void *bp_history)
{
    DPRINTF(Perceptron, "BP_Top entered squash\n");
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    // Delete this BPHistory now that we're done with it.
    delete history;
}

void
PerceptronBP_Top::reset()
{

}

void 
PerceptronBP_Top::uncondBr(void * &bp_history)
{
    BPHistory *history = new BPHistory;
    history->perceptron_y = 1; //anything greater than 0 is taken
	  bp_history = static_cast<void *>(history);
}

inline int8_t
PerceptronBP_Top::changeToPlusMinusOne(int32_t input)
{
  return (input > 0) ? 1 : -1;
}
