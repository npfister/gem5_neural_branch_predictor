/*
* Authors: Alex Ionescu, Nick Pfister
*/

#include "base/intmath.hh"
#include "base/misc.hh"
#include "base/trace.hh"
#include "debug/Fetch.hh"
#include "cpu/pred/perceptron.hh"
#include "cpu/pred/perceptron_top.hh"

PerceptronBP_Top::PerceptronBP_Top(unsigned globalPredictorSize, unsigned globalHistBits, int8_t theta)
{

	this->globalPredictorSize = globalPredictorSize;
	this->globalHistBits = globalHistBits;

	if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid perceptron table size!\n");
    }


	for(int i=0;i < globalPredictorSize; i++) { //create perceprton table
		this->perceptronTable.push_back(new PerceptronBP(globalHistBits));
	}

	this->X.push_back(1);
	for(int i=1;i < globalHistBits; i++) { //
		this->X.push_back(0);
	}

	this->globalHistoryMask = (unsigned)(power(2,globalPredictorSize) - 1);

	this->theta = theta;

}

bool
PerceptronBP_Top::lookup(Addr &branch_addr, void * &bp_history)
{

	PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & this->globalHistoryMask];
	BPHistory *history = new BPHistory;
	history->perceptron_y = curr_perceptron->getPrediction(this->X);
	bp_history = static_cast<void *>(history);

	// y 
	return (history->perceptron_y) > 0;

}

void
PerceptronBP_Top::BTBUpdate(Addr &branch_addr, void * &bp_history)
{

}

void
PerceptronBP_Top::update(Addr &branch_addr, bool taken, void *bp_history)
{

	PerceptronBP* curr_perceptron = this->perceptronTable[ (branch_addr >> 2) & this->globalHistoryMask];

	curr_perceptron->train((int8_t)((taken > 0) ? 1 : -1), static_cast<BPHistory *>(bp_history)->perceptron_y, this->theta);

}

void
PerceptronBP_Top::squash(void *bp_history)
{
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    // Delete this BPHistory now that we're done with it.
    delete history;

    assert(bp_history == NULL);
}

void
PerceptronBP_Top::reset()
{

}
