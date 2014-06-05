#ifndef __CORRELATE_H__
#define __CORRELATE_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
#endif
