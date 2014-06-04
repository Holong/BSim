#ifndef __CORRELATE_H__
#define __CORRELATE_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"
#include "predictor/common.h"

#define COR_GLOBAL_BITS			2
#define COR_GLOBAL_SIZE			(1 << COR_GLOBAL_BITS)
#define COR_GLOBAL_MASK			(COR_GLOBAL_SIZE - 1)
#define COR_getGlobalOffset(x)		((x) & COR_GLOBAL_MASK)

#define COR_BTB_BITS			10
#define COR_BTB_SIZE			(1 << COR_BTB_BITS)
#define COR_BTB_MASK			(COR_BTB_SIZE - 1)
#define COR_getBTBOffset(x)			((x) & COR_BTB_MASK)

#define COR_COUNTER_BITS			COR_BTB_BITS
#define COR_COUNTER_SIZE			(1 << COR_COUNTER_BITS)
#define COR_COUNTER_MASK			(COR_COUNTER_SIZE - 1)
#define COR_getCounterOffset(x)		((x) & COR_COUNTER_MASK)

class Correlate : public BPredictor
{
private:
	unsigned long *branchTargetBuffer;
	enum _state (*counter)[COR_COUNTER_SIZE];
	int global;

public:
	Correlate();
	~Correlate();
	Correlate(const Correlate& p);

	virtual void do_predict(unsigned long ip, BranchResult& result);
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success);
};

Correlate::Correlate()
{
	sprintf(this->name, "Correlating Branch");
	global = 0;

	branchTargetBuffer = new unsigned long[COR_BTB_SIZE];
	memset(branchTargetBuffer, 0, COR_BTB_SIZE * sizeof(unsigned long));

	counter = new enum _state[COR_GLOBAL_SIZE][COR_COUNTER_SIZE];
	memset(counter, 0, COR_GLOBAL_SIZE * COR_COUNTER_SIZE * sizeof(enum _state));
}

Correlate::~Correlate()
{
	delete []branchTargetBuffer;
	delete []counter;
}

Correlate::Correlate(const Correlate& p)
{
	branchTargetBuffer = new unsigned long[COR_BTB_SIZE];

	for(int i = 0; i < COR_BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}

	counter = new enum _state[COR_GLOBAL_SIZE][COR_BTB_SIZE];

	for(int i = 0; i < COR_GLOBAL_SIZE; i++)
	{
		for(int j = 0; j < COR_BTB_SIZE; j++)
		{
			counter[i][j] = p.counter[i][j];
		}
	}
}

void Correlate::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);

	if(InstType(type) == CALL || InstType(type) == RETURN || InstType(type) == JMP)
	{
		result.setTorF(true);
		result.setJmpAddress(branchTargetBuffer[COR_getBTBOffset(ip)]);
	}
	else
	{
		if(counter[COR_getGlobalOffset(global)][COR_getBTBOffset(ip)] == TT || counter[COR_getGlobalOffset(global)][COR_getBTBOffset(ip)] == T)
		{
			result.setTorF(true);
			result.setJmpAddress(branchTargetBuffer[COR_getBTBOffset(ip)]);
		}
		else
		{
			result.setTorF(false);
		}
	}
}

void Correlate::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);

	branchTargetBuffer[COR_getBTBOffset(ip)] = nextIP;

	if(InstType(type) == CND_JMP)
	{
		if(success)
		{
			if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == TT)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = TT;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == T)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = TT;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == N)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = T;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == NN)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = N;

			global = global << 1 | 1;
		}
		else
		{
			if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == TT)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = T;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == T)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = N;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == N)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = NN;
			else if(counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] == NN)
				counter[COR_getGlobalOffset(global)][COR_getCounterOffset(ip)] = NN;

			global = global << 1 | 0;
		}
	}
}

#endif
