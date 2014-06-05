#ifndef __GSHARE_RET_H__
#define __GSHARE_RET_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
#endif
