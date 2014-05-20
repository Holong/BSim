#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <stdlib.h>
#include <unistd.h>

#include "predictor.h"
#include "predictor/notTaken.h"

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
	double getMissRate();
};

Profiler::Profiler(pid_t pid, BPredictor* branchPredictor)
{
	this->pid = pid;

	beforeIP = 0;
	currentIP = 0;
	nextIP = 0;
	predictedIP = 0;

	beforeBranch = false;

	totalNumOfInst = 0;
	totalNumOfBranchInst = 0;
	totalNumOfMissPredict = 0;

	pPredictor = branchPredictor;
}

void Profiler::setIP(unsigned long ip)
{
	totalNumOfInst++;

	nextIP = ip;

	if(beforeBranch) {
		if(nextIP != predictedIP)
			totalNumOfMissPredict++;
	}

	currentIP = ip;

	Disassembler disAssem;

	if(disAssem.isBranch(currentIP, pid)) {

		totalNumOfBranchInst++;
		beforeBranch = true;

		BranchResult result;

		pPredictor->do_predict(currentIP, result);

		if(result.getTorF()) {
			predictedIP = result.getJmpAddress();
		}
		else {
			predictedIP = currentIP + disAssem.getInstLen(ip, pid);
		}
	}
	else {
		beforeBranch = false;
	}

	printf("[totalInst : %ld, totalBranch : %ld, totalMiss : %ld]\n", totalNumOfInst, totalNumOfBranchInst, totalNumOfMissPredict);
}

double Profiler::getMissRate()
{
	return (double)totalNumOfMissPredict/(double)totalNumOfBranchInst;
}

#endif
