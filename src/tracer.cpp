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

void Tracer::traceSingleStep(InfoRegs& pInfoRegs) throw (int)
{
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
		return;
	}
	
	pInfoRegs.setRegsInfo(childPid);
}

pid_t Tracer::getChildPid()
{
	return childPid;
}

InfoRegs::InfoRegs()
{
	r15 = 0; r14 = 0; r13 = 0;
	r12 = 0; rbp = 0; rbx = 0;
	r11 = 0; r10 = 0; r9 = 0;
	r8 = 0;  rax = 0; rcx = 0;
	rdx = 0; rsi = 0; rdi = 0;
	orig_rax = 0;     rip = 0;
	cs = 0;  eflags = 0;
	rsp = 0; ss = 0;  fs_base = 0;
	gs_base = 0;	ds = 0;
	es = 0; fs = 0;	gs = 0;
}

void InfoRegs::setRegsInfo(pid_t pid) throw (int)
{
	r15 = 	ptrace(PTRACE_PEEKUSER, pid, 0 * 8, 0);
	r14 = 	ptrace(PTRACE_PEEKUSER, pid, 1 * 8, 0);
	r13 = 	ptrace(PTRACE_PEEKUSER, pid, 2 * 8, 0);
	r12 = 	ptrace(PTRACE_PEEKUSER, pid, 3 * 8, 0);
	rbp = 	ptrace(PTRACE_PEEKUSER, pid, 4 * 8, 0);
	rbx = 	ptrace(PTRACE_PEEKUSER, pid, 5 * 8, 0);
	r11 = 	ptrace(PTRACE_PEEKUSER, pid, 6 * 8, 0);
	r10 = 	ptrace(PTRACE_PEEKUSER, pid, 7 * 8, 0);
	r9 = 	ptrace(PTRACE_PEEKUSER, pid, 8 * 8, 0);
	r8 = 	ptrace(PTRACE_PEEKUSER, pid, 9 * 8, 0);
	rax = 	ptrace(PTRACE_PEEKUSER, pid, 10 * 8, 0);
	rcx = 	ptrace(PTRACE_PEEKUSER, pid, 11 * 8, 0);
	rdx = 	ptrace(PTRACE_PEEKUSER, pid, 12 * 8, 0);
	rsi = 	ptrace(PTRACE_PEEKUSER, pid, 13 * 8, 0);
	rdi = 	ptrace(PTRACE_PEEKUSER, pid, 14 * 8, 0);
	orig_rax = ptrace(PTRACE_PEEKUSER, pid, 15 * 8, 0);
	rip = 	ptrace(PTRACE_PEEKUSER, pid, 16 * 8, 0);
	cs = 	ptrace(PTRACE_PEEKUSER, pid, 17 * 8, 0);
	eflags = ptrace(PTRACE_PEEKUSER, pid, 18 * 8, 0);
	rsp = 	ptrace(PTRACE_PEEKUSER, pid, 19 * 8, 0);
	ss = 	ptrace(PTRACE_PEEKUSER, pid, 20 * 8, 0);
	fs_base = ptrace(PTRACE_PEEKUSER, pid, 21 * 8, 0);
	gs_base = ptrace(PTRACE_PEEKUSER, pid, 22 * 8, 0);
	ds = 	ptrace(PTRACE_PEEKUSER, pid, 23 * 8, 0);
	es = 	ptrace(PTRACE_PEEKUSER, pid, 24 * 8, 0);
	fs = 	ptrace(PTRACE_PEEKUSER, pid, 25 * 8, 0);
	gs = 	ptrace(PTRACE_PEEKUSER, pid, 26 * 8, 0);
}

unsigned long InfoRegs::getRIP()
{
	return rip;
}
