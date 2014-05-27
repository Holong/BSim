#include <stdio.h>

#include "profiler.h"
#include "disassemble.h"

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
}

void Profiler::toString()
{
	printf("[%s] [totalInst : %ld, totalBranch : %ld, totalMiss : %ld]\n", pPredictor->nameOfPredictor(), totalNumOfInst, totalNumOfBranchInst, totalNumOfMissPredict);
}

double Profiler::getMissRate()
{
	return (double)totalNumOfMissPredict/(double)totalNumOfBranchInst;
}
