#ifndef __TRACER_H__
#define __TRACER_H__

#include <sys/ptrace.h>
#include <unistd.h>

class InfoRegs
{
private:
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long rbp;
	unsigned long rbx;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rax;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rsi;
	unsigned long rdi;
	unsigned long orig_rax;
	unsigned long rip;
	unsigned long cs;
	unsigned long eflags;
	unsigned long rsp;
	unsigned long ss;
	unsigned long fs_base;
	unsigned long gs_base;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;

public:
	InfoRegs();
	void setRegsInfo(pid_t pid) throw (int);
	unsigned long getRIP();
};

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
	void traceSingleStep(InfoRegs& pInfoRegs) throw (int);
	pid_t getChildPid();
};

#endif
