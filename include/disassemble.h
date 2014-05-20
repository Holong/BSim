#ifndef __DISASSEMBLE_H__
#define __DISASSEMBLE_H__

#include "distorm.h"

class Disassembler
{
private:
	long buf[2];
	_DecodedInst inst1;
	_DInst inst2;
	unsigned int resultIntCount;
	char FCbuf[20];

	_CodeInfo ci;

	void analyze(unsigned long ip, pid_t pid) throw (int);
public:
	Disassembler();
	void showInst(unsigned long ip, pid_t pid) throw (int);
	int isBranch(unsigned long ip, pid_t pid) throw (int);
	unsigned int getInstLen(unsigned long ip, pid_t pid) throw (int);
};

#endif
