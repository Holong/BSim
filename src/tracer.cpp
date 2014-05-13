#include <string.h>
#include <iostream>
#include <sys/wait.h>

#include "tracer.h"

using namespace std;

Tracer::Tracer(char* programName)
{
	childPid = 0;
	stat = 0;
	res = 0;
	signo = 0;

	fileName = strrchr(programName, '/');
	if(fileName != NULL)
		fileName ++;
	else
		fileName = programName;

	absoluteName = programName;
}

void Tracer::traceStart() throw (int)
{
	if( (childPid = fork()) == 0)
	{
		childPid = getpid();

		cout << "Attaching to process " << childPid << endl;
		
		if( (res = ptrace(PTRACE_TRACEME, 0, 0, 0)) != 0 ) {
			throw res;
		}
		execl(absoluteName, fileName, (char*)NULL);
	}
	res = waitpid(childPid, &stat, WUNTRACED);

	if ((res != childPid) || !(WIFSTOPPED(stat)) ) {
		throw res;
	}

	cout << "Wait result stat " << stat << " childPid " << res << endl;
	stat = 0;
	signo = 0;
}

long Tracer::traceSingleStep() throw (int)
{
	int ipoffs = 16 * 8;
	long ip;

	if( (res = ptrace(PTRACE_SINGLESTEP, childPid, 0, signo)) < 0) {
		throw res;
	}
	res = wait(&stat);

	if( (signo = WSTOPSIG(stat)) == SIGTRAP ) {
		signo = 0;
	}
	if( (signo == SIGHUP) || (signo == SIGINT) ) {
		ptrace(PTRACE_CONT, childPid, 0, signo);
		cout << "Child took a SIGHUP or SIGINT. We are done" << endl;
		return 0;
	}
	
	ip = ptrace(PTRACE_PEEKUSER, childPid, ipoffs, 0);
	
	return ip;
}

pid_t Tracer::getChildPid()
{
	return childPid;
}
