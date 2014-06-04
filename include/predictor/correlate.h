#ifndef __CORRELATE_H__
#define __CORRELATE_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"
#include "predictor/common.h"

#define GLOBAL_BITS			2
#define GLOBAL_SIZE			(1 << GLOBAL_BITS)
#define GLOBAL_MASK			(GLOBAL_SIZE - 1)
#define getGlobalOffset(x)		((x) & GLOBAL_MASK)

#define BTB_BITS			10
#define BTB_SIZE			(1 << BTB_BITS)
#define BTB_MASK			(BTB_SIZE - 1)
#define getBTBOffset(x)			((x) & BTB_MASK)

#define COUNTER_BITS			BTB_BITS
#define COUNTER_SIZE			(1 << COUNTER_BITS)
#define COUNTER_MASK			(COUNTER_SIZE - 1)
#define getCounterOffset(x)		((x) & COUNTER_MASK)

class Correlate : public BPredictor
{
private:
	unsigned long *branchTargetBuffer;
	enum _state (*counter)[COUNTER_SIZE];
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

	branchTargetBuffer = new unsigned long[BTB_SIZE];
	memset(branchTargetBuffer, 0, BTB_SIZE * sizeof(unsigned long));

	counter = new enum _state[GLOBAL_SIZE][COUNTER_SIZE];
	memset(counter, 0, GLOBAL_SIZE * COUNTER_SIZE * sizeof(enum _state));
}

Correlate::~Correlate()
{
	delete []branchTargetBuffer;
	delete []counter;
}

Correlate::Correlate(const Correlate& p)
{
	branchTargetBuffer = new unsigned long[BTB_SIZE];

	for(int i = 0; i < BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}

	counter = new enum _state[GLOBAL_SIZE][BTB_SIZE];

	for(int i = 0; i < GLOBAL_SIZE; i++)
	{
		for(int j = 0; j < BTB_SIZE; j++)
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
		result.setJmpAddress(branchTargetBuffer[getBTBOffset(ip)]);
	}
	else
	{
		if(counter[getGlobalOffset(global)][getBTBOffset(ip)] == TT || counter[getGlobalOffset(global)][getBTBOffset(ip)] == T)
		{
			result.setTorF(true);
			result.setJmpAddress(branchTargetBuffer[getBTBOffset(ip)]);
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

	branchTargetBuffer[getBTBOffset(ip)] = nextIP;

	if(InstType(type) == CND_JMP)
	{
		if(success)
		{
			if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == TT)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = TT;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == T)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = TT;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == N)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = T;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == NN)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = N;

			global = global << 1 | 1;
		}
		else
		{
			if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == TT)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = T;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == T)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = N;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == N)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = NN;
			else if(counter[getGlobalOffset(global)][getCounterOffset(ip)] == NN)
				counter[getGlobalOffset(global)][getCounterOffset(ip)] = NN;

			global = global << 1 | 0;
		}
	}
}

#endif
