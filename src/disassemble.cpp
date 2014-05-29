#include <iostream>
#include <sys/ptrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_functions.h"
#include "disassemble.h"
#include "distorm.h"

using std::cout;
using std::cin;
using std::endl;

Disassembler::Disassembler()
{
	memset(buf, 0, sizeof(buf)/sizeof(long));
	memset(&inst1, 0, sizeof(inst1));
	memset(&inst2, 0, sizeof(inst2));
	resultIntCount = 0;
	memset(FCbuf, 0, sizeof(FCbuf)/sizeof(char));
	memset(&ci, 0, sizeof(ci));

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
	unsigned int type = 0;

	
	type = typeOfInst(ip, pid);

	switch(type & 0xF) {
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

	printf("IP : 0x%016lx\t[%02d] %-23s %-12s%s%-35s\t%-20s [%x, %x]\n",
			ip,
			inst1.size,
			(char*)inst1.instructionHex.p,
			(char*)inst1.mnemonic.p,
			inst1.operands.length != 0 ? " " : "",
			(char*)inst1.operands.p,
			FCbuf, InstType(type), BranchType(type));
}

unsigned int Disassembler::isBranch() throw (int)
{
	int result;

	result = META_GET_FC(inst2.meta);

	if(result == 3 || result == 7)	// predicate & syscall 
		result = 0;

	if(result > 7 || result < 0)
		throw 1;

	return result;
}

unsigned int Disassembler::getInstLen(unsigned long ip, pid_t pid) throw (int)
{
	analyze(ip, pid);
	
	return inst1.size;
}

int Disassembler::getOperand() throw (int)
{
	bool imm = false;
	bool reg = false;
	bool relative = false;
	bool memory = false;

	int result = 0;

	for(int i = 0; i < 4; i++)
	{
		switch(inst2.ops[i].type)
		{
			case 1:
				reg = true;
				break;
			case 2:
			case 3:
			case 4:
				imm = true;
				break;
			case 5:
			case 6:
			case 7:
			case 9:
				memory = true;
				break;
			case 8:
				relative = true;
				break;
			default:
				break;
		}
	}

	if(memory)
		result = MEMORY;
	else if(reg)
		result = REG;
	else if(relative)
		result = DIRECT;
	else if(imm)
		result = DIRECT;
	else 
		result = BRANCHTYPENUM;

	return result;
}

unsigned int Disassembler::typeOfInst(unsigned long ip, pid_t pid) throw (int)
{
	unsigned long result = 0;
	
	analyze(ip, pid);
	
	result = isBranch();
	switch(result)
	{
		case NOT:
			break;
		case CALL:
			result |= getOperand() << 4;
			break;
		case RETURN:
			result |= MEMORY << 4;
			break;
		case SYSCALL:
			result |= REG << 4;
			break;
		case JMP:
			result |= getOperand() << 4; 
			break;
		case CND_JMP:
			result |= getOperand() << 4;
			break;
		case INT:
			result |= DIRECT << 4;
			break;
		case PRE:
			result |= REG << 4;
			break;
		default:
			throw 1;
	}

	return result;
}
