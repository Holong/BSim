#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <iostream>

#include "distorm.h"
#include "mnemonics.h"
#include "tracer.h"
#include "error_functions.h"

using std::endl;
using std::cout;
using std::cin;

int main (int argc, char *argv[]) 
{
	long ip;	

	long buf[2];
	_DecodedInst temp;
	_DInst temp2;
	unsigned int result_inst_count;
	char FCbuf[20];

	_CodeInfo ci;
	ci.code = (const uint8_t*)buf;
	ci.codeLen = sizeof(buf);
	ci.codeOffset = 0;
	ci.dt = Decode64Bits;

	/*
	 ** This program is started with the PID of the target process.
	 */
	try {
		if (argc != 2)
			throw 1;
	}
	catch(int exept) {
		usageErr("%s app_name\n", argv[0]);
	}

	Tracer tracer(argv[1]);

	try {
		tracer.traceStart();
	}
	catch(int exept) {
		errExit("trace start");
	}

	/*
	 ** Loop now, tracing the child
	 */
	while (1) {
		/*
		 ** Ok, now we will continue the child, but set the single step bit in
		 ** the psw. This will cause the child to exeute just one instruction and
		 ** trap us again. The wait(2) catches the trap.
		 */ 		
		try {
			ip = tracer.traceSingleStep();
		}
		catch(int exept) {
			errExit("Single step");
		}

		buf[0] = ptrace(PTRACE_PEEKTEXT, tracer.getChildPid(), ip, 0);
		buf[1] = ptrace(PTRACE_PEEKTEXT, tracer.getChildPid(), ip + 8, 0);

		if(distorm_decode(0, (const unsigned char*)buf, 16, Decode64Bits, &temp, 1, &result_inst_count) == DECRES_INPUTERR) {
			printf("Input error\n");
			exit(1);
		}
		if(result_inst_count != 1)
			printf("error\n");

		distorm_decompose(&ci, &temp2, 1, &result_inst_count);
		if(result_inst_count != 1)
			printf("error\n");

		switch(META_GET_FC(temp2.meta)) {
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
				temp.size,
				(char*)temp.instructionHex.p,
				(char*)temp.mnemonic.p,
				temp.operands.length != 0 ? " " : "",
				(char*)temp.operands.p,
				FCbuf);
	}
	sleep(5);
	return(0);
}
