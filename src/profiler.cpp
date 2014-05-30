#include <stdio.h>

#include "profiler.h"
#include "disassemble.h"

Profiler::Profiler(pid_t pid, BPredictor* branchPredictor, Disassembler* disAssem, char* argv[])
{
	this->pid = pid;
	this->argv = argv;

	beforeIP = 0;
	currentIP = 0;
	nextIP = 0;
	predictedIP = 0;

	beforeResult = 0;
	currentResult = 0;

	totalNumOfInst = 0;
	totalNumOfBranchInst = 0;
	totalNumOfMissPredict = 0;

	time = 0;

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

void Profiler::setTime(long time)
{
	this->time = time;
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

void Profiler::printRawData()
{
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

void Profiler::printResult()
{
	printf("\n\n");
	printf(" Simulation Result \n\n");

	printf("  1. Predictor : %s\n", pPredictor->nameOfPredictor());
	printf("  2. Workload  : ");
	for(int i = 0; ; i++) {
		if(argv[i] != NULL) {
			printf("%s ", argv[i]);
		}
		else {
			printf("\n");
			break;
		}
	}
	printf("  3. Time : %ld.%ld s\n\n", time/1000000, time%1000000);

	printf("  4. Result\n");
	printf("     1) Summary\n");
	printf("        Total Instructions : %10ld\n", totalNumOfInst);
	printf("              Branches     : %10ld\n", totalNumOfBranchInst);
	printf("              Miss Rate    : %10lf\n\n", getTotalMissRate());
	
	printf("     2) Direction\n");
	printf("        Direct Branches    : %10ld\n", getNumOfDirectInst());
	printf("            Miss Rate      : %10lf\n", getDirectInstMissRate());
	printf("            Direction Miss : %10lf\n", getDirectDirectionMissRate());
	printf("            Address Miss   : %10lf\n\n", getDirectAddressMissRate());

	printf("        Indirect Branches  : %10ld\n", getNumOfIndirectInst());
	printf("            Miss Rate      : %10lf\n", getIndirectInstMissRate());
	printf("            Direction Miss : %10lf\n", getIndirectDirectionMissRate());
	printf("            Address Miss   : %10lf\n\n", getIndirectAddressMissRate());

	
	printf("     3) Condition\n");
	printf("        Cond Branches      : %10ld\n", getNumOfCondInst());
	printf("            Miss Rate      : %10lf\n", getCondInstMissRate());
	printf("            Direction Miss : %10lf\n", getCondDirectionMissRate());
	printf("            Address Miss   : %10lf\n\n", getCondAddressMissRate());

	printf("        Uncond Branches    : %10ld\n", getNumOfUncondInst());
	printf("            Miss Rate      : %10lf\n", getUncondInstMissRate());
	printf("            Direction Miss : %10lf\n", getUncondDirectionMissRate());
	printf("            Address Miss   : %10lf\n\n", getUncondAddressMissRate());

}

double Profiler::getTotalMissRate()
{
	return (double)totalNumOfMissPredict/(double)totalNumOfBranchInst;
}

unsigned long Profiler::getNumOfDirectInst()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[DIRECT].numOfInst;
	}

	return result;
}

unsigned long Profiler::getNumOfDirectDirectionMiss()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[DIRECT].numOfMissPredictDirection;
	}

	return result;
}

unsigned long Profiler::getNumOfDirectAddressMiss()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[DIRECT].numOfMissPredictAddress;
	}

	return result;
}

unsigned long Profiler::getNumOfIndirectInst()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[REG].numOfInst;
		result += data[i].typeOfBranch[MEMORY].numOfInst;
	}

	return result;
}

unsigned long Profiler::getNumOfIndirectDirectionMiss()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[REG].numOfMissPredictDirection;
		result += data[i].typeOfBranch[MEMORY].numOfMissPredictDirection;
	}

	return result;
}

unsigned long Profiler::getNumOfIndirectAddressMiss()
{
	unsigned long result = 0;

	for(int i = 1; i < TYPENUM; i++)
	{
		result += data[i].typeOfBranch[REG].numOfMissPredictAddress;
		result += data[i].typeOfBranch[MEMORY].numOfMissPredictAddress;
	}

	return result;
}


double Profiler::getDirectInstMissRate()
{
	return ((double)getNumOfDirectDirectionMiss() +	(double)getNumOfDirectAddressMiss()) / (double)getNumOfDirectInst();
}

double Profiler::getDirectDirectionMissRate()
{
	return (double)getNumOfDirectDirectionMiss() / ((double)getNumOfDirectDirectionMiss() +
			(double)getNumOfDirectAddressMiss());
}

double Profiler::getDirectAddressMissRate()
{
	return (double)getNumOfDirectAddressMiss() / ((double)getNumOfDirectDirectionMiss() +
			(double)getNumOfDirectAddressMiss());
}

double Profiler::getIndirectInstMissRate()
{
	return ((double)getNumOfIndirectDirectionMiss() + (double)getNumOfIndirectAddressMiss()) / (double)getNumOfIndirectInst();
}

double Profiler::getIndirectDirectionMissRate()
{
	return (double)getNumOfIndirectDirectionMiss() / ((double)getNumOfIndirectDirectionMiss() +
			(double)getNumOfIndirectAddressMiss());
}

double Profiler::getIndirectAddressMissRate()
{
	return (double)getNumOfIndirectAddressMiss() / ((double)getNumOfIndirectDirectionMiss() +
			(double)getNumOfIndirectAddressMiss());
}








unsigned long Profiler::getNumOfCondInst()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CND_JMP].typeOfBranch[i].numOfInst;
	}

	return result;
}

unsigned long Profiler::getNumOfCondDirectionMiss()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CND_JMP].typeOfBranch[i].numOfMissPredictDirection;
	}

	return result;
}

unsigned long Profiler::getNumOfCondAddressMiss()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CND_JMP].typeOfBranch[i].numOfMissPredictAddress;
	}

	return result;
}

unsigned long Profiler::getNumOfUncondInst()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CALL].typeOfBranch[i].numOfInst;
		result += data[RETURN].typeOfBranch[i].numOfInst;
		result += data[JMP].typeOfBranch[i].numOfInst;
		result += data[INT].typeOfBranch[i].numOfInst;
	}

	return result;
}

unsigned long Profiler::getNumOfUncondDirectionMiss()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CALL].typeOfBranch[i].numOfMissPredictDirection;
		result += data[RETURN].typeOfBranch[i].numOfMissPredictDirection;
		result += data[JMP].typeOfBranch[i].numOfMissPredictDirection;
		result += data[INT].typeOfBranch[i].numOfMissPredictDirection;
	}

	return result;
}

unsigned long Profiler::getNumOfUncondAddressMiss()
{
	unsigned long result = 0;

	for(int i = 0; i < BRANCHTYPENUM; i++)
	{
		result += data[CALL].typeOfBranch[i].numOfMissPredictAddress;
		result += data[RETURN].typeOfBranch[i].numOfMissPredictAddress;
		result += data[JMP].typeOfBranch[i].numOfMissPredictAddress;
		result += data[INT].typeOfBranch[i].numOfMissPredictAddress;
	}

	return result;
}


double Profiler::getCondInstMissRate()
{
	return ((double)getNumOfCondDirectionMiss() + (double)getNumOfCondAddressMiss()) / (double)getNumOfCondInst();
}

double Profiler::getCondDirectionMissRate()
{
	return (double)getNumOfCondDirectionMiss() / ((double)getNumOfCondDirectionMiss() +
			(double)getNumOfCondAddressMiss());
}

double Profiler::getCondAddressMissRate()
{
	return (double)getNumOfCondAddressMiss() / ((double)getNumOfCondDirectionMiss() +
			(double)getNumOfCondAddressMiss());
}

double Profiler::getUncondInstMissRate()
{
	return ((double)getNumOfUncondDirectionMiss() + (double)getNumOfUncondAddressMiss()) / (double)getNumOfUncondInst();
}

double Profiler::getUncondDirectionMissRate()
{
	return (double)getNumOfUncondDirectionMiss() / ((double)getNumOfUncondDirectionMiss() +
			(double)getNumOfUncondAddressMiss());
}

double Profiler::getUncondAddressMissRate()
{
	return (double)getNumOfUncondAddressMiss() / ((double)getNumOfUncondDirectionMiss() +
			(double)getNumOfUncondAddressMiss());
}

