#include "predictor/notTaken.h"

void NotTaken::do_predict(unsigned long ip, BranchResult& result)
{
	Disassembler disAssem;

	result.setTorF(false);
	result.setJmpAddress(ip + disAssem.getInstLen(ip, pid));
}

void NotTaken::after_predict(unsigned long ip, unsigned long nextIP, bool success)
{
}
