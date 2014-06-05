#ifndef __GSHARE_H__
#define __GSHARE_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
#endif
