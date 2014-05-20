#include <iostream>
#include <sys/ptrace.h>
#include <stdio.h>

#include "disassemble.h"
#include "distorm.h"

using std::cout;
using std::cin;
using std::endl;

Disassembler::Disassembler()
{
	ci.code = (const uint8_t*)buf;
	ci.codeLen = sizeof(buf);
	ci.codeOffset = 0;
	ci.dt = Decode64Bits;
}

void Disassembler::analyze(unsigned long ip, pid_t pid) throw (int)
{
	buf[0] = ptrace(PTRACE_PEEKTEXT, pid, ip, 0);
	buf[1] = ptrace(PTRACE_PEEKTEXT, pid, ip + 8, 0);

	if(distorm_decode(0, (const unsigned char*)buf, 16, ci.dt, &inst1, 1, &resultIntCount) == DECRES_INPUTERR) 
		throw 1;

	if(resultIntCount != 1) {
		cout << "distorm_decode error" << endl;
		throw 1;
	}

	distorm_decompose(&ci, &inst2, 1, &resultIntCount);

	if(resultIntCount != 1) {
		cout << "distorm_decompose error" << endl;
		throw 1;
	}
}

void Disassembler::showInst(unsigned long ip, pid_t pid) throw (int)
{
	analyze(ip, pid);

	switch(META_GET_FC(inst2.meta)) {
		case FC_NONE:
			sprintf(FCbuf, "FC_NONE");
			break;
		case FC_CALL:
			sprintf(FCbuf, "FC_CALL");
			break;
		case FC_RET:
			sprintf(FCbuf, "FC_RET");
			break;
		case FC_SYS:
			sprintf(FCbuf, "FC_SYS");
			break;
		case FC_UNC_BRANCH:
			sprintf(FCbuf, "FC_UNC_BRANCH");
			break;
		case FC_CND_BRANCH:
			sprintf(FCbuf, "FC_CND_BRANCH");
			break;
		case FC_INT:
			sprintf(FCbuf, "FC_INT");
			break;
		case FC_CMOV:
			sprintf(FCbuf, "FC_CMOV");
			break;
	}

	printf("IP : 0x%016lx\t[%02d] %-30s %s%s%-35s\t%s\n",
			ip,
			inst1.size,
			(char*)inst1.instructionHex.p,
			(char*)inst1.mnemonic.p,
			inst1.operands.length != 0 ? " " : "",
			(char*)inst1.operands.p,
			FCbuf);
}

int Disassembler::isBranch(unsigned long ip, pid_t pid) throw (int)
{
	int result;

	analyze(ip, pid);

	result = META_GET_FC(inst2.meta);

	if(result > 7 || result < 0)
		throw 1;
	return result;
}

unsigned int Disassembler::getInstLen(unsigned long ip, pid_t pid) throw (int)
{
	analyze(ip, pid);
	
	return inst1.size;
}
