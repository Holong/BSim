#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <stdlib.h>
#include <unistd.h>

#include "predictor.h"

class Profiler
{
private:
	pid_t pid;

	unsigned long beforeIP;
	unsigned long currentIP;
	unsigned long nextIP;
	unsigned long predictedIP;

	BPredictor* pPredictor;

	bool beforeBranch;

	unsigned long totalNumOfInst;
	unsigned long totalNumOfBranchInst;
	unsigned long totalNumOfMissPredict;

	unsigned long getNextInstFromPredictor(unsigned long ip);
	
public:
	Profiler(pid_t pid, BPredictor* branchPredictor);
	void setIP(unsigned long ip);
	void toString();
	double getMissRate();
};

#endif
