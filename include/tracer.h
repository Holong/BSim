#ifndef __TRACER_H__
#define __TRACER_H__

#include <sys/ptrace.h>
#include <unistd.h>


class Tracer
{
private:
	pid_t childPid;
	int stat, res;
	int signo;
	char* fileName;
	char* absoluteName;
	
public:
	Tracer(char* programName);
	void traceStart() throw (int);
	long traceSingleStep() throw (int);
	pid_t getChildPid();
};

#endif
