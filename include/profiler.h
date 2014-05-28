#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <stdlib.h>
#include <unistd.h>

#include "predictor.h"

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
	unsigned long numOfNotBranch;
	unsigned long numOfCall;
	unsigned long numOfReturn;
	unsigned long numOfSyscall;
	unsigned long numOfJump;
	unsigned long numOfCondJump;
	unsigned long numOfInterrupt;
	unsigned long numOfPredicate;

	unsigned long totalNumOfMissPredict;
	unsigned long numOfMissPredictDirection;
	unsigned long numOfMissPredictAddress;
	unsigned long numOfMissPredictBoth;

	unsigned long numOfCondDirectBranch;
	unsigned long numOfCondIndirectBranch;
	unsigned long numOfUnCondDirectBranch;
	unsigned long numOfUnCondIndirectBranch;



	unsigned long getNextInstFromPredictor(unsigned long ip);
	
public:
	Profiler(pid_t pid, BPredictor* branchPredictor);
	void setIP(unsigned long ip);
	void toString();
	double getMissRate();
};

#endif
