#include "predictor/2_bit.h"

TwoBit::TwoBit()
{
	sprintf(this->name, "2-bit Predictor");

	state = new enum _state[TWO_BIT_BTB_SIZE];
	branchTargetBuffer = new unsigned long[TWO_BIT_BTB_SIZE];
	memset(branchTargetBuffer, 0, TWO_BIT_BTB_SIZE * sizeof(unsigned long));
	memset(state, 0, TWO_BIT_BTB_SIZE * sizeof(enum _state));
}

TwoBit::~TwoBit()
{
	delete []branchTargetBuffer;
	delete []state;
}

TwoBit::TwoBit(const TwoBit& p) : BPredictor(p)
{
	branchTargetBuffer = new unsigned long[TWO_BIT_BTB_SIZE];
	
	for(int i = 0; i < TWO_BIT_BTB_SIZE; i++)
	{
		branchTargetBuffer[i] = p.branchTargetBuffer[i];
	}

	state = new enum _state[TWO_BIT_BTB_SIZE];
	
	for(int i = 0; i < TWO_BIT_BTB_SIZE; i++)
	{
		state[i] = p.state[i];
	}
}

void TwoBit::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);

	if(InstType(type) == CALL || InstType(type) == RETURN || InstType(type) == JMP)
	{
		result.setTorF(true);
		result.setJmpAddress(branchTargetBuffer[TWO_BIT_getBTBOffset(ip)]);
	}
	else
	{
		if(state[TWO_BIT_getBTBOffset(ip)] == TT || state[TWO_BIT_getBTBOffset(ip)] == T)
		{
			result.setTorF(true);
			result.setJmpAddress(branchTargetBuffer[TWO_BIT_getBTBOffset(ip)]);
		}
		else
		{
			result.setTorF(false);
		}
	}
}

void TwoBit::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
	Disassembler disAssem;
	unsigned int type = disAssem.typeOfInst(ip, pid);

	branchTargetBuffer[TWO_BIT_getBTBOffset(ip)] = nextIP;

	if(InstType(type) == CND_JMP)
	{
		if(success)
		{
			if(state[TWO_BIT_getBTBOffset(ip)] == TT)
				state[TWO_BIT_getBTBOffset(ip)] = TT;
			else if(state[TWO_BIT_getBTBOffset(ip)] == T)
				state[TWO_BIT_getBTBOffset(ip)] = TT;
			else if(state[TWO_BIT_getBTBOffset(ip)] == N)
				state[TWO_BIT_getBTBOffset(ip)] = T;
			else if(state[TWO_BIT_getBTBOffset(ip)] == NN)
				state[TWO_BIT_getBTBOffset(ip)] = N;
		}
		else
		{
			if(state[TWO_BIT_getBTBOffset(ip)] == TT)
				state[TWO_BIT_getBTBOffset(ip)] = T;
			else if(state[TWO_BIT_getBTBOffset(ip)] == T)
				state[TWO_BIT_getBTBOffset(ip)] = N;
			else if(state[TWO_BIT_getBTBOffset(ip)] == N)
				state[TWO_BIT_getBTBOffset(ip)] = NN;
			else if(state[TWO_BIT_getBTBOffset(ip)] == NN)
				state[TWO_BIT_getBTBOffset(ip)] = NN;
		}
	}
}
