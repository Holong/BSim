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
#include "disassemble.h"

using std::endl;
using std::cout;
using std::cin;

int main (int argc, char *argv[]) 
{
	unsigned long ip;	

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
	
	Disassembler disAssem;
	InfoRegs infoRegs;
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
			tracer.traceSingleStep(infoRegs);
			ip = infoRegs.getRIP();
		}
		catch(int exept) {
			errExit("Single step");
		}
		
		disAssem.ShowInst(ip, tracer.getChildPid());
	}
	sleep(5);
	return(0);
}
