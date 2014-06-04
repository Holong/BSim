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
#include <sys/time.h>

#include "distorm.h"
#include "mnemonics.h"
#include "tracer.h"
#include "error_functions.h"
#include "disassemble.h"
#include "simul.h"

#include "profiler.h"
#include "predictor.h"
#include "predictor/common.h"

extern int errno;

using std::endl;
using std::cout;
using std::cin;

int main (int argc, char *argv[]) 
{
	unsigned long ip;	
	struct timeval startTime;
	struct timeval endTime;

	/*
	 ** This program is started with the PID of the target process.
	 */
	try {
		if (argc < 2)
			throw 1;
	}
	catch(int exept) {
		usageErr("%s app_name\n", argv[0]);
		exit(1);
	}
	
	Disassembler* disAssem = new Disassembler;
	InfoRegs infoRegs;

	// select profiled program
	Tracer tracer(argv[1], &argv[1]);

	try {
		tracer.traceStart();
	}
	catch(int exept) {
		errExit("trace start");
	}
	
	// set Simulator
	Simul bSim(tracer.getChildPid(), disAssem, &argv[1]);

	// set predictor
	bSim.setPredictor(new NotTaken());
	bSim.setPredictor(new TwoBit());
	bSim.setPredictor(new Correlate());

	gettimeofday(&startTime, NULL);
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
			bSim.runSimulation(ip);
		}
		catch(int execpt) {
			errMsg("runSimulation\n");
		}
			
		try {
//			disAssem->showInst(ip, tracer.getChildPid());
		}
		catch(int ex) {
			errMsg("showInst");
		}

	}
	gettimeofday(&endTime, NULL);
	long endSec = (long)endTime.tv_sec * 1000000 + endTime.tv_usec;
	long startSec = (long)startTime.tv_sec * 1000000 + startTime.tv_usec;
	bSim.setTime(endSec - startSec);

	bSim.printResult(true);
	return 0;
}
