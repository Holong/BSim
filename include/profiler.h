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
	Disassembler* pDisAssem;
	BranchResult predictedResult;
	char** argv;

	unsigned long beforeIP;
	unsigned long currentIP;
	unsigned long nextIP;
	unsigned long predictedIP;

	unsigned int beforeResult;
	unsigned int currentResult;

	unsigned long totalNumOfInst;
	unsigned long totalNumOfBranchInst;
	unsigned long totalNumOfMissPredict;

	struct type_of_inst data[TYPENUM];

	long time;



	unsigned long getNextInstFromPredictor(unsigned long ip);
	
	unsigned long getNumOfDirectInst();
	unsigned long getNumOfDirectDirectionMiss();
	unsigned long getNumOfDirectAddressMiss();
	double getDirectInstMissRate();
	double getDirectDirectionMissRate();
	double getDirectAddressMissRate();

	unsigned long getNumOfIndirectInst();
	unsigned long getNumOfIndirectDirectionMiss();
	unsigned long getNumOfIndirectAddressMiss();
	double getIndirectInstMissRate();
	double getIndirectDirectionMissRate();
	double getIndirectAddressMissRate();

	unsigned long getNumOfCondInst();
	unsigned long getNumOfCondDirectionMiss();
	unsigned long getNumOfCondAddressMiss();
	double getCondInstMissRate();
	double getCondDirectionMissRate();
	double getCondAddressMissRate();

	unsigned long getNumOfUncondInst();
	unsigned long getNumOfUncondDirectionMiss();
	unsigned long getNumOfUncondAddressMiss();
	double getUncondInstMissRate();
	double getUncondDirectionMissRate();
	double getUncondAddressMissRate();

public:
	Profiler(pid_t pid, BPredictor* branchPredictor, Disassembler* disAssem, char* argv[]);
	void setTime(long time);
	void setIP(unsigned long ip);
	void printResult();
	void printRawData();
	double getTotalMissRate();
};

#endif
