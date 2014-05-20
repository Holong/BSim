#ifndef __NOT_TAKEN_H__
#define __NOT_TAKEN_H__

#include <iostream>
#include <unistd.h>

#include "predictor.h"

class NotTaken : public BPredictor
{
public:
	NotTaken(pid_t pid) : BPredictor(pid) { };
	virtual void do_predict(unsigned long ip, BranchResult& result);
};

void NotTaken::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;

	result.setTorF(true);
	result.setJmpAddress(ip + disAssem.getInstLen(ip, pid));
}

#endif
