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
#include <errno.h>

#include "distorm.h"
#include "mnemonics.h"
#include "tracer.h"
#include "error_functions.h"
#include "disassemble.h"

#include "profiler.h"
#include "predictor/notTaken.h"

extern int errno;

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
		exit(1);
	}
	
	Disassembler* disAssem = new Disassembler;
	InfoRegs infoRegs;

	// select profiled program
	Tracer tracer(argv[1]);

	try {
		tracer.traceStart();
	}
	catch(int exept) {
		errExit("trace start");
	}
	
	// set predictor
	BPredictor* predictor = new NotTaken(tracer.getChildPid());
	Profiler profiler(tracer.getChildPid(), predictor, disAssem);

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
		catch(int except) {
			if(errno == ESRCH) {
				cout << "Target Process is terminated...." << endl;
				break;
			}
			errMsg("Single step");
			break;
		}
		
		try {
		//	disAssem->showInst(ip, tracer.getChildPid());
		}
		catch(int ex) {
			errMsg("showInst");
		}

		try {
			profiler.setIP(ip);
		}
		catch(int execpt) {
			errMsg("setIP\n");
		}
	}
	profiler.toString();
	printf("Miss Rate : %lf\n", profiler.getMissRate());
	return 0;
}
