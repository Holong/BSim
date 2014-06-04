#ifndef __GSHARE_H__
#define __GSHARE_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"
#include "predictor/common.h"

#define GSHARE_GLOBAL_BITS			10
#define GSHARE_GLOBAL_SIZE			(1 << GSHARE_GLOBAL_BITS)
#define GSHARE_GLOBAL_MASK			(GSHARE_GLOBAL_SIZE - 1)
#define GSHARE_getGlobalOffset(x)		((x) & GSHARE_GLOBAL_MASK)

#define GSHARE_BTB_BITS				10	
#define GSHARE_BTB_SIZE				(1 << GSHARE_BTB_BITS)
#define GSHARE_BTB_MASK				(GSHARE_BTB_SIZE - 1)
#define GSHARE_getBTBOffset(x)			((x) & GSHARE_BTB_MASK)

#define GSHARE_COUNTER_BITS			GSHARE_BTB_BITS
#define GSHARE_COUNTER_SIZE			(1 << GSHARE_COUNTER_BITS)
#define GSHARE_COUNTER_MASK			(GSHARE_COUNTER_SIZE - 1)
#define GSHARE_getCounterOffset(x)		((x) & GSHARE_COUNTER_MASK)

class GShare : public BPredictor
{
private:
	unsigned long *branchTargetBuffer;
	enum _state *counter;
	int branchHistoryReg;

public:
	GShare();
	~GShare();
	GShare(const GShare& p);

	virtual void do_predict(unsigned long ip, BranchResult& result);
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success);
};

GShare::GShare()
{
	sprintf(this->name, "gshare predictor");
	branchHistoryReg = 0;

	branchTargetBuffer = new unsigned long[GSHARE_BTB_SIZE];
	memset(branchTargetBuffer, 0, GSHARE_BTB_SIZE * sizeof(unsigned long));

	counter = new enum _state[GSHARE_COUNTER_SIZE];
	memset(counter, 0, GSHARE_COUNTER_SIZE * sizeof(enum _state));
}

GShare::~GShare()
{
	delete []branchTargetBuffer;
	delete []counter;
}

GShare::GShare(const GShare& p)
{
	branchTargetBuffer = new unsigned long[GSHARE_BTB_SIZE];

	for(int i = 0; i < GSHARE_BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}

	counter = new enum _state[GSHARE_BTB_SIZE];

	for(int i = 0; i < GSHARE_BTB_SIZE; i++)
	{
		counter[i] = p.counter[i];
	}
}

void GShare::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);
	int index = GSHARE_getBTBOffset(ip) ^ GSHARE_getGlobalOffset(branchHistoryReg);

	if(InstType(type) == CALL || InstType(type) == RETURN || InstType(type) == JMP)
	{
		result.setTorF(true);
		result.setJmpAddress(branchTargetBuffer[GSHARE_getBTBOffset(ip)]);
	}
	else
	{
		if(counter[index] == TT || counter[index] == T)
		{
			result.setTorF(true);
			result.setJmpAddress(branchTargetBuffer[GSHARE_getBTBOffset(ip)]);
		}
		else
		{
			result.setTorF(false);
		}
	}
}

void GShare::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);
	int index = GSHARE_getBTBOffset(ip) ^ GSHARE_getGlobalOffset(branchHistoryReg);

	branchTargetBuffer[GSHARE_getBTBOffset(ip)] = nextIP;

	if(InstType(type) == CND_JMP)
	{
		if(success)
		{
			if(counter[index] == TT)
				counter[index] = TT;
			else if(counter[index] == T)
				counter[index] = TT;
			else if(counter[index] == N)
				counter[index] = T;
			else if(counter[index] == NN)
				counter[index] = N;

			branchHistoryReg = branchHistoryReg << 1 | 1;
		}
		else
		{
			if(counter[index] == TT)
				counter[index] = T;
			else if(counter[index] == T)
				counter[index] = N;
			else if(counter[index] == N)
				counter[index] = NN;
			else if(counter[index] == NN)
				counter[index] = NN;

			branchHistoryReg = branchHistoryReg << 1 | 0;
		}
	}
}

#endif
