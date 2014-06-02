#ifndef __SIMUL_H__
#define __SIMUL_H__

#include <unistd.h>

#include "profiler.h"
#include "predictor.h"
#include "disassemble.h"

#define MAX_SIZE	100

class Simul
{
private:
	Profiler* pProfiler[MAX_SIZE];
	int cnt;
	pid_t pid;
	Disassembler* disAssem;
	char** argv;

public:
	Simul(pid_t pid, Disassembler* disAssem, char* argv[]);
	void setPredictor(BPredictor* pPredictor);
	void runSimulation(unsigned long ip);
	void setTime(long time);
	void printResult(bool raw);
};

Simul::Simul(pid_t pid, Disassembler* disAssem, char* argv[])
{
	this->pid = pid;
	this->disAssem = disAssem;
	this->argv = argv;

	for(int i = 0; i < MAX_SIZE; i++)
		pProfiler[i] = NULL;
	
	cnt = -1;
}

void Simul::setPredictor(BPredictor* pPredictor)
{
	cnt++;

	pPredictor->setPid(pid);
	pProfiler[cnt] = new Profiler(pid, pPredictor, disAssem, argv);
}

void Simul::runSimulation(unsigned long ip)
{
	for(int i = 0; i <= cnt; i++)
	{
		pProfiler[i]->setIP(ip);
	}
}

void Simul::setTime(long time)
{
	for(int i = 0; i <= cnt; i++)
	{
		pProfiler[i]->setTime(time);
	}
}

void Simul::printResult(bool raw)
{
	for(int i = 0; i <= cnt; i++)
	{
		pProfiler[i]->printResult();

		if(raw)
			pProfiler[i]->printRawData();
	}
}

#endif
