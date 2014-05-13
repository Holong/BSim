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

#include "distorm.h"
#include "mnemonics.h"

int main (int argc, char *argv[]) 
{

	pid_t pid;
	int stat, res;
	int signo;
	long ip;
	int ipoffs;

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
	if (argc != 2) {
		printf("Need a program of traced process\n");
		printf("Usage: %s program\n", argv[0]);
		exit(1);
	}

	if((pid = fork()) == 0)
	{
		char* filename;

		printf("Attaching to process %d\n", getpid());
		if ((res = ptrace(PTRACE_TRACEME, 0, 0, 0)) != 0) {;
			printf("Attach result %d\n",res);
		}

		filename = strrchr(argv[1], '/');
		if(filename != NULL)
			filename++;
		else
			filename = argv[1];

		execl(argv[1], filename, (char*) NULL);
	}
	res = waitpid(pid, &stat, WUNTRACED);

	/*
	 ** Get the offset into the user area of the IP and SP registers. We'll
	 ** need this later.
	 */
	ipoffs = 16*8;

	/*
	 ** Attach to the process. This will cause the target process to become
	 ** the child of this process. The target will be sent a SIGSTOP. call
	 ** wait(2) after this to detect the child state change. We're expecting
	 ** the new child state to be STOPPED
	 */
	if ((res != pid) || !(WIFSTOPPED(stat)) ) {
		printf("Unexpected wait result res %d stat %x\n",res,stat);
		exit(1);
	}
	printf("Wait result stat %x pid %d\n",stat, res);
	stat = 0;
	signo = 0;

	/*
	 ** Loop now, tracing the child
	 */
	while (1) {
		/*
		 ** Ok, now we will continue the child, but set the single step bit in
		 ** the psw. This will cause the child to exeute just one instruction and
		 ** trap us again. The wait(2) catches the trap.
		 */ 		
		if ((res = ptrace(PTRACE_SINGLESTEP, pid, 0, signo)) < 0) {
			perror("Ptrace singlestep error");
			exit(1);
		}
		res = wait(&stat);
		/*
		 ** The previous call to wait(2) returned the child's signal number.
		 ** If this is a SIGTRAP, then we set it to zero (this does not get
		 ** passed on to the child when we PTRACE_CONT or PTRACE_SINGLESTEP
		 ** it).  If it is the SIGHUP, then PTRACE_CONT the child and we 
		 ** can exit.
		 */
		if ((signo = WSTOPSIG(stat)) == SIGTRAP) {
			signo = 0;
		}
		if ((signo == SIGHUP) || (signo == SIGINT)) {
			ptrace(PTRACE_CONT, pid, 0, signo);
			printf("Child took a SIGHUP or SIGINT. We are done\n");
			break;
		}
		/*
		 ** Fetch the current IP and SP from the child's user area. Log them.
		 */

		ip = ptrace(PTRACE_PEEKUSER, pid, ipoffs, 0);
		buf[0] = ptrace(PTRACE_PEEKTEXT, pid, ip, 0);
		buf[1] = ptrace(PTRACE_PEEKTEXT, pid, ip + 8, 0);

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
