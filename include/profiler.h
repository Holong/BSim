#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <stdlib.h>
#include <unistd.h>

#include "predictor.h"
#include "disassemble.h"

struct statistics
{
	unsigned long numOfInst;
	unsigned long numOfSuccess;
	unsigned long numOfMissPredictDirection;
	unsigned long numOfMissPredictAddress;
	unsigned long numOfMissPredictBoth;
};

struct type_of_inst
{
	struct statistics typeOfBranch[BRANCHTYPENUM];
};

class Profiler
{
private:
	pid_t pid;
	BPredictor* pPredictor;

	bool beforeBranch;
	unsigned long beforeIP;
	unsigned long currentIP;
	unsigned long nextIP;
	unsigned long predictedIP;

	unsigned long totalNumOfInst;
	unsigned long totalNumOfBranchInst;
	unsigned long totalNumOfMissPredict;

	struct type_of_inst data[TYPENUM];

	unsigned long getNextInstFromPredictor(unsigned long ip);
	
public:
	Profiler(pid_t pid, BPredictor* branchPredictor);
	void setIP(unsigned long ip);
	void toString();
	double getMissRate();
};

#endif
