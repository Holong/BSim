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
	enum _state state;

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
	state = TT;

	branchTargetBuffer = new unsigned long[BTB_SIZE];
	memset(branchTargetBuffer, 0, BTB_SIZE * sizeof(unsigned long));
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

	if(state == TT || state == T)
	{
		result.setTorF(true);
		result.setJmpAddress(branchTargetBuffer[getBTBOffset(ip)]);
	}
	else
	{
		result.setTorF(false);
	}
}

void TwoBit::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	if(success)
	{
		if(state == TT)
			state = TT;
		else if(state == T)
			state = TT;
		else if(state == N)
			state = T;
		else if(state == NN)
			state = N;
	}
	else
	{
		if(state == TT)
			state = T;
		else if(state == T)
			state = N;
		else if(state == N)
			state = NN;
		else if(state == NN)
			state = NN;
	}

	branchTargetBuffer[getBTBOffset(ip)] = nextIP;
}

#endif
