#ifndef __NOT_TAKEN_H__
#define __NOT_TAKEN_H__

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "disassemble.h"
#include "predictor.h"

class NotTaken : public BPredictor
{
public:
	NotTaken(pid_t pid) : BPredictor(pid) {
		sprintf(this->name, "NotTaken");
	};
	virtual void do_predict(unsigned long ip, BranchResult& result);
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success);
};

void NotTaken::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;

	result.setTorF(false);
	result.setJmpAddress(ip + disAssem.getInstLen(ip, pid));
}

void NotTaken::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
}

#endif
