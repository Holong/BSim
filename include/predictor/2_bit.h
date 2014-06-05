#ifndef __2_BIT_H__
#define __2_BIT_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"
#include "predictor/common.h"

#define TWO_BIT_SIZE				10
#define TWO_BIT_BTB_SIZE			(1 << TWO_BIT_SIZE)
#define TWO_BIT_MASK				(TWO_BIT_BTB_SIZE - 1)
#define TWO_BIT_getBTBOffset(x)				((x) & TWO_BIT_MASK)

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
#endif
