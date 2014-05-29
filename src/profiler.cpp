#include <stdio.h>

#include "profiler.h"
#include "disassemble.h"

Profiler::Profiler(pid_t pid, BPredictor* branchPredictor, Disassembler* disAssem)
{
	this->pid = pid;

	beforeIP = 0;
	currentIP = 0;
	nextIP = 0;
	predictedIP = 0;

	beforeResult = 0;
	currentResult = 0;

	totalNumOfInst = 0;
	totalNumOfBranchInst = 0;
	totalNumOfMissPredict = 0;

	for(int i = 0; i < TYPENUM; i++)
	{
		for(int j = 0; j < BRANCHTYPENUM; j++)
		{
			data[i].typeOfBranch[j].numOfInst = 0;
			data[i].typeOfBranch[j].numOfSuccess = 0;
			data[i].typeOfBranch[j].numOfMissPredictDirection = 0;
			data[i].typeOfBranch[j].numOfMissPredictAddress = 0;
		}
	}

	pPredictor = branchPredictor;
	pDisAssem = disAssem;
}

void Profiler::setIP(unsigned long ip)
{
	bool xdirection;
	totalNumOfInst++;

	nextIP = ip;

	if(beforeResult) {
		if(nextIP != predictedIP) {
			totalNumOfMissPredict++;
			
			xdirection = false;

			if(nextIP != currentIP + pDisAssem->getInstLen(currentIP, pid)) {
				if(!predictedResult.getTorF()) {
					xdirection = true;
				}
			}
			else {
				if(predictedResult.getTorF()) {
					xdirection = true;
				}
			}
			
			if(xdirection) {
				data[InstType(beforeResult)].typeOfBranch[BranchType(beforeResult)].numOfMissPredictDirection++;
			}
			else
			{
				data[InstType(beforeResult)].typeOfBranch[BranchType(beforeResult)].numOfMissPredictAddress++;
			}

		}
		else {
			data[InstType(beforeResult)].typeOfBranch[BranchType(beforeResult)].numOfSuccess++;
		}
	}

	currentIP = ip;
	currentResult = pDisAssem->typeOfInst(currentIP, pid);
	beforeResult = currentResult;

	if(currentResult) {
		totalNumOfBranchInst++;
		data[InstType(currentResult)].typeOfBranch[BranchType(currentResult)].numOfInst++;

		pPredictor->do_predict(currentIP, predictedResult);

		if(predictedResult.getTorF()) {
			predictedIP = predictedResult.getJmpAddress();
		}
		else {
			predictedIP = currentIP + pDisAssem->getInstLen(currentIP, pid);
		}
	}
	else {
		data[InstType(currentResult)].typeOfBranch[BranchType(currentResult)].numOfInst++;
	}
}

void Profiler::toString()
{
	printf("[%s] [totalInst : %ld, totalBranch : %ld, totalMiss : %ld]\n\n", pPredictor->nameOfPredictor(), totalNumOfInst, totalNumOfBranchInst, totalNumOfMissPredict);

	printf("=========================== NOT BRANCH ============================= \n");
	printf("TOTAL   ");
	printf("%15ld\n", data[NOT].typeOfBranch[DIRECT].numOfInst);
	
	printf("\n=========================== CALL INST ============================== \n");
	printf("        %15s%15s%15s%15s\n", "Total Inst", "Success", "MissDirection", "MissAddress");
	for(int j = 0; j < BRANCHTYPENUM; j++)
	{	
		switch(j) {
			case DIRECT:
				printf("DIRECT  ");
				break;
			case REG:
				printf("REGISTER");
				break;
			case MEMORY:
				printf("MEMORY  ");
				break;
		}
		printf("%15ld", data[CALL].typeOfBranch[j].numOfInst);
		printf("%15ld", data[CALL].typeOfBranch[j].numOfSuccess);
		printf("%15ld", data[CALL].typeOfBranch[j].numOfMissPredictDirection);
		printf("%15ld\n", data[CALL].typeOfBranch[j].numOfMissPredictAddress);
	}
	
	printf("\n============================= RETURN =============================== \n");
	printf("        %15s%15s%15s%15s\n", "Total Inst", "Success", "MissDirection", "MissAddress");
	for(int j = 0; j < BRANCHTYPENUM; j++)
	{	
		switch(j) {
			case DIRECT:
				printf("DIRECT  ");
				break;
			case REG:
				printf("REGISTER");
				break;
			case MEMORY:
				printf("MEMORY  ");
				break;
		}
		printf("%15ld", data[RETURN].typeOfBranch[j].numOfInst);
		printf("%15ld", data[RETURN].typeOfBranch[j].numOfSuccess);
		printf("%15ld", data[RETURN].typeOfBranch[j].numOfMissPredictDirection);
		printf("%15ld\n", data[RETURN].typeOfBranch[j].numOfMissPredictAddress);
	}

	printf("\n============================ JUMP INST ============================= \n");
	printf("        %15s%15s%15s%15s\n", "Total Inst", "Success", "MissDirection", "MissAddress");
	for(int j = 0; j < BRANCHTYPENUM; j++)
	{	
		switch(j) {
			case DIRECT:
				printf("DIRECT  ");
				break;
			case REG:
				printf("REGISTER");
				break;
			case MEMORY:
				printf("MEMORY  ");
				break;
		}
		printf("%15ld", data[JMP].typeOfBranch[j].numOfInst);
		printf("%15ld", data[JMP].typeOfBranch[j].numOfSuccess);
		printf("%15ld", data[JMP].typeOfBranch[j].numOfMissPredictDirection);
		printf("%15ld\n", data[JMP].typeOfBranch[j].numOfMissPredictAddress);
	}

	printf("\n===================== CONDITIONAL JUMP INST ======================== \n");
	printf("        %15s%15s%15s%15s\n", "Total Inst", "Success", "MissDirection", "MissAddress");
	for(int j = 0; j < BRANCHTYPENUM; j++)
	{	
		switch(j) {
			case DIRECT:
				printf("DIRECT  ");
				break;
			case REG:
				printf("REGISTER");
				break;
			case MEMORY:
				printf("MEMORY  ");
				break;
		}
		printf("%15ld", data[CND_JMP].typeOfBranch[j].numOfInst);
		printf("%15ld", data[CND_JMP].typeOfBranch[j].numOfSuccess);
		printf("%15ld", data[CND_JMP].typeOfBranch[j].numOfMissPredictDirection);
		printf("%15ld\n", data[CND_JMP].typeOfBranch[j].numOfMissPredictAddress);
	}

	printf("\n========================== INTERRUPT INST ========================== \n");
	printf("        %15s%15s%15s%15s\n", "Total Inst", "Success", "MissDirection", "MissAddress");
	for(int j = 0; j < BRANCHTYPENUM; j++)
	{	
		switch(j) {
			case DIRECT:
				printf("DIRECT  ");
				break;
			case REG:
				printf("REGISTER");
				break;
			case MEMORY:
				printf("MEMORY  ");
				break;
		}
		printf("%15ld", data[INT].typeOfBranch[j].numOfInst);
		printf("%15ld", data[INT].typeOfBranch[j].numOfSuccess);
		printf("%15ld", data[INT].typeOfBranch[j].numOfMissPredictDirection);
		printf("%15ld\n", data[INT].typeOfBranch[j].numOfMissPredictAddress);
	}
}

double Profiler::getMissRate()
{
	return (double)totalNumOfMissPredict/(double)totalNumOfBranchInst;
}
