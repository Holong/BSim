#ifndef __2_BIT_H__
#define __2_BIT_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"

#define SIZE				10
#define BTB_SIZE			(1 << SIZE)
#define MASK				(BTB_SIZE - 1)
#define getBTBOffset(x)			((x) & MASK)

enum _state {TT = 0, T, N, NN};

class TwoBit : public BPredictor
{
private:
	unsigned long* branchTargetBuffer;
	enum _state* state;

public:
	TwoBit();
	~TwoBit();
	TwoBit(const TwoBit& p);

	virtual void do_predict(unsigned long ip, BranchResult& result);
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success);
};

TwoBit::TwoBit()
{
	sprintf(this->name, "2-bit Predictor");

	state = new enum _state[BTB_SIZE];
	branchTargetBuffer = new unsigned long[BTB_SIZE];
	memset(branchTargetBuffer, 0, BTB_SIZE * sizeof(unsigned long));
	memset(state, 0, BTB_SIZE * sizeof(enum _state));
}

TwoBit::~TwoBit()
{
	delete []branchTargetBuffer;
}

TwoBit::TwoBit(const TwoBit& p) : BPredictor(p)
{
	branchTargetBuffer = new unsigned long[BTB_SIZE];
	
	for(int i = 0; i < BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}
}

void TwoBit::do_predict(unsigned long ip, BranchResult& result)
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
		if(state[getBTBOffset(ip)] == TT || state[getBTBOffset(ip)] == T)
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

void TwoBit::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	if(success)
	{
		if(state[getBTBOffset(ip)] == TT)
			state[getBTBOffset(ip)] = TT;
		else if(state[getBTBOffset(ip)] == T)
			state[getBTBOffset(ip)] = TT;
		else if(state[getBTBOffset(ip)] == N)
			state[getBTBOffset(ip)] = T;
		else if(state[getBTBOffset(ip)] == NN)
			state[getBTBOffset(ip)] = N;
	}
	else
	{
		if(state[getBTBOffset(ip)] == TT)
			state[getBTBOffset(ip)] = T;
		else if(state[getBTBOffset(ip)] == T)
			state[getBTBOffset(ip)] = N;
		else if(state[getBTBOffset(ip)] == N)
			state[getBTBOffset(ip)] = NN;
		else if(state[getBTBOffset(ip)] == NN)
			state[getBTBOffset(ip)] = NN;
	}

	branchTargetBuffer[getBTBOffset(ip)] = nextIP;
}

#endif
