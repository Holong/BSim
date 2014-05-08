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

#define M_OFFSETOF(STRUCT, ELEMENT) \
	&((STRUCT *)NULL)->ELEMENT;

#define D_LINUXNONUSRCONTEXT 0x40000000

int main (int argc, char *argv[]) 
{

	pid_t pid;
	int stat, res;
	int signo;
	int ip, sp;
	int ipoffs, spoffs;
	int initialSP = -1;
	int initialIP = -1;
	struct user u_area;


	/*
	 ** This program is started with the PID of the target process.
	 */
	if (argv[1] == NULL) {
		printf("Need a program of traced process\n");
		printf("Usage: %s program\n", argv[0]);
		exit(1);
	}

	if((pid = fork()) == 0)
	{
		char* filename;

		printf("Attaching to process %d\n", getpid());
		if ((ptrace(PTRACE_TRACEME, 0, 0, 0)) != 0) {;
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
//	ipoffs = static_cast<int>(M_OFFSETOF(struct user, regs.rip));
//	spoffs = static_cast<int>(M_OFFSETOF(struct user, regs.rsp));
	ipoffs = 16*8;
	spoffs = 19*8;

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
		sleep(1);
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
		sp = ptrace(PTRACE_PEEKUSER, pid, spoffs, 0);
		/*
		 ** Checkto see where we are in the process. Using the ldd(1) utility, I
		 ** dumped the list of shared libraries that were required by this process.
		 ** This showed:
		 **
		 **     libc.so.6 => /lib/i686/libc.so.6 (0x40030000)
		 **     /lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x40000000)
		 **
		 ** So basically, we can assume that any execuable address pointed to by
		 ** the IP that is *over* 0x40000000 is either in ld.so, libc.so, or in
		 ** some sort of kernel state. We really don't care about these addresses
		 ** so we'll skip 'em. Also, nm(1) showed that all the local symbols
		 ** we would be interested in start in the 0x08000000 range.
		 */
		if (ip & D_LINUXNONUSRCONTEXT) {
			continue;
		} 
		if (initialIP == -1) {
			initialIP = ip;
			initialSP = sp;
			printf("---- Starting LOOP IP %x SP %x ---- \n",
					initialIP, initialSP);
		} else {
			if ((ip == initialIP) && (sp == initialSP)) {
				ptrace(PTRACE_CONT, pid, 0, signo);
				printf("----- LOOP COMPLETE -----\n");
				break;
			}
		}
		printf("Stat %x IP %x SP %x  Last signal %d\n",stat, ip, sp,
				signo);
		/*
		 ** If we're back to where we started tracing the loop, then exit
		 */
	}
	printf("Debugging complete\n");

	sleep(5);
	return(0);
}
