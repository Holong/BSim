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
#include <signal.h>

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

Simul* pbSim;
char* workLoad[100];

void int_handler(int sig)
{
	pbSim->printResult(true);
	exit(1);
}

void workLoadParsing(char* arg)
{
	char* temp = (char*)malloc(sizeof(char) * 100);
	int index = 0;
	int tempIndex = 0;
	int pointerIndex = 0;

	while(1)
	{
		if(arg[index] == '\0')
		{
			workLoad[pointerIndex] = temp;
			pointerIndex++;
			workLoad[pointerIndex] = NULL;
			break;
		}
		else if(arg[index] == ' ')
		{
			workLoad[pointerIndex] = temp;
			pointerIndex++;
			index++;
			tempIndex = 0;

			temp = (char*)malloc(sizeof(char) * 100);
		}
		else
		{
			temp[tempIndex] = arg[index];
			index++;
			tempIndex++;
		}
	}
}

int main (int argc, char *argv[]) 
{
	unsigned long ip;	

	Disassembler* disAssem = new Disassembler;
	InfoRegs infoRegs;

	struct timeval startTime;
	struct timeval endTime;

	int opt;
	unsigned long numOfLimitInst = 0;
	unsigned long numOfBranchInst = 0;
	bool limitInst = false;
	char** temp = NULL;

	bool debugOn = false;

	bool rawOn = false;
	
	bool loadOn = false;

	/*
	 ** This program is started with the PID of the target process.
	 */

	while((opt = getopt(argc, argv, ":i:drl:")) != -1)
	{
		switch(opt) {
			case 'i':
				numOfLimitInst = strtoul(optarg, temp, 0);
				limitInst = true;
				break;
			case 'd':
				debugOn = true;
				break;
			case 'r':
				rawOn = true;
				break;
			case 'l':
				loadOn = true;
				workLoadParsing(argv[optind - 1]);
				for(int k = 0; ; k++)
				{
					if(workLoad[k] == NULL)
						break;
					else
						printf("%s\n", workLoad[k]);
				}
				break;				
		}
	}

	try {
		if(!loadOn)
			throw 1;
	}
	catch (int ex) {
		errExit("Not WorkLoad");
	}

	struct sigaction act;
	act.sa_handler = int_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	// select profiled program
	Tracer tracer(workLoad[0], &workLoad[0]);

	try {
		tracer.traceStart();
	}
	catch(int exept) {
		errExit("trace start");
	}
	
	// set Simulator
	Simul bSim(tracer.getChildPid(), disAssem, &workLoad[0]);
	pbSim = &bSim;

	// set predictor
	bSim.setPredictor(new NotTaken());
	bSim.setPredictor(new TwoBit());
	bSim.setPredictor(new Correlate());
	bSim.setPredictor(new GShare());

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

		if(debugOn)
		{
			try {
				disAssem->showInst(ip, tracer.getChildPid());
			}
			catch(int ex) {
				errMsg("showInst");
			}
		}

		if(limitInst)
		{
			if(disAssem->typeOfInst(ip, tracer.getChildPid()))
			{
				numOfBranchInst++;

				if(numOfBranchInst >= numOfLimitInst)
					break;
			}
		}
	}
	gettimeofday(&endTime, NULL);
	long endSec = (long)endTime.tv_sec * 1000000 + endTime.tv_usec;
	long startSec = (long)startTime.tv_sec * 1000000 + startTime.tv_usec;
	bSim.setTime(endSec - startSec);

	bSim.printResult(rawOn);
	return 0;
}
