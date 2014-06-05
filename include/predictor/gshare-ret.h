#ifndef __GSHARE_RET_H__
#define __GSHARE_RET_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"
#include "predictor/common.h"

#define GSHARE_RET_GLOBAL_BITS			10
#define GSHARE_RET_GLOBAL_SIZE			(1 << GSHARE_RET_GLOBAL_BITS)
#define GSHARE_RET_GLOBAL_MASK			(GSHARE_RET_GLOBAL_SIZE - 1)
#define GSHARE_RET_getGlobalOffset(x)		((x) & GSHARE_RET_GLOBAL_MASK)

#define GSHARE_RET_BTB_BITS			10	
#define GSHARE_RET_BTB_SIZE			(1 << GSHARE_RET_BTB_BITS)
#define GSHARE_RET_BTB_MASK			(GSHARE_RET_BTB_SIZE - 1)
#define GSHARE_RET_getBTBOffset(x)		((x) & GSHARE_RET_BTB_MASK)

#define GSHARE_RET_COUNTER_BITS			GSHARE_RET_BTB_BITS
#define GSHARE_RET_COUNTER_SIZE			(1 << GSHARE_RET_COUNTER_BITS)
#define GSHARE_RET_COUNTER_MASK			(GSHARE_RET_COUNTER_SIZE - 1)
#define GSHARE_RET_getCounterOffset(x)		((x) & GSHARE_RET_COUNTER_MASK)

#define GSHARE_RET_STACK_SIZE			8

class GShareRet : public BPredictor
{
private:
	unsigned long *branchTargetBuffer;
	enum _state *counter;
	int branchHistoryReg;

	unsigned long *retStack;
	int sp;
	int cntFalseRet;

public:
	GShareRet();
	~GShareRet();
	GShareRet(const GShareRet& p);

	virtual void do_predict(unsigned long ip, BranchResult& result);
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success);
};

GShareRet::GShareRet()
{
	sprintf(this->name, "gshare-ret predictor");
	branchHistoryReg = 0;

	branchTargetBuffer = new unsigned long[GSHARE_RET_BTB_SIZE];
	memset(branchTargetBuffer, 0, GSHARE_RET_BTB_SIZE * sizeof(unsigned long));

	counter = new enum _state[GSHARE_RET_COUNTER_SIZE];
	memset(counter, 0, GSHARE_RET_COUNTER_SIZE * sizeof(enum _state));

	retStack = new unsigned long[GSHARE_RET_STACK_SIZE];
	memset(counter, 0, GSHARE_RET_STACK_SIZE * sizeof(unsigned long));
	sp = -1;
	cntFalseRet = 0;
}

GShareRet::~GShareRet()
{
	delete []branchTargetBuffer;
	delete []counter;
	delete []retStack;
}

GShareRet::GShareRet(const GShareRet& p)
{
	branchTargetBuffer = new unsigned long[GSHARE_RET_BTB_SIZE];

	for(int i = 0; i < GSHARE_RET_BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}

	counter = new enum _state[GSHARE_RET_BTB_SIZE];

	for(int i = 0; i < GSHARE_RET_BTB_SIZE; i++)
	{
		counter[i] = p.counter[i];
	}

	retStack = new unsigned long[GSHARE_RET_STACK_SIZE];

	for(int i = 0; i < GSHARE_RET_STACK_SIZE; i++)
	{
		retStack[i] = p.retStack[i];
	}
}

void GShareRet::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);
	int index = GSHARE_RET_getBTBOffset(ip) ^ GSHARE_RET_getGlobalOffset(branchHistoryReg);

	if(InstType(type) == CALL || InstType(type) == JMP)
	{
		result.setTorF(true);
		result.setJmpAddress(branchTargetBuffer[GSHARE_RET_getBTBOffset(ip)]);
	}
	else if(InstType(type) == RETURN)
	{
		result.setTorF(true);

		if(sp >= 0)
		{
			result.setJmpAddress(retStack[sp]);
			sp--;
			if(sp < 0)
				sp = GSHARE_RET_STACK_SIZE - 1;
		}
		else
		{
			result.setJmpAddress(0);
		}
	}
	else
	{
		if(counter[index] == TT || counter[index] == T)
		{
			result.setTorF(true);
			result.setJmpAddress(branchTargetBuffer[GSHARE_RET_getBTBOffset(ip)]);
		}
		else
		{
			result.setTorF(false);
		}
	}
}

void GShareRet::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);
	int index = GSHARE_RET_getBTBOffset(ip) ^ GSHARE_RET_getGlobalOffset(branchHistoryReg);


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

		branchTargetBuffer[GSHARE_RET_getBTBOffset(ip)] = nextIP;
	}
	else if(InstType(type) == CALL)
	{
		sp = (sp + 1) % GSHARE_RET_STACK_SIZE;
		retStack[sp] = ip + disAssem.getInstLen(ip, pid);
		branchTargetBuffer[GSHARE_RET_getBTBOffset(ip)] = nextIP;
	}
	else if(InstType(type) == RETURN)
	{
		if(!success)
		{
			cntFalseRet++;
			
			if(cntFalseRet > GSHARE_RET_STACK_SIZE / 2)
			{
				sp = -1;
				cntFalseRet = 0;
			}
		}
	}
	else
	{
		branchTargetBuffer[GSHARE_RET_getBTBOffset(ip)] = nextIP;
	}
}

#endif
