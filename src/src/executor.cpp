#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <seccomp.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include "logger.h"


template<class T> unsigned long long int __PAIR__(unsigned int high, T low) { return (((unsigned long long int)high) << sizeof(high)*8) | (unsigned int)(low); }

int read_line(char *buf, int length) {
	for(int i = 0; i < length; i++) {
		if(read(0, buf, 1) != 1)
			return -1;
		if(*buf == '\n')
			break;
		++buf;
	}
	*buf = 0;
	return 0;
}
int read_int() {
	char buf[16] = {0, };
	read_line(buf, 16);
	return atoi(buf);
}
int load_elf() {
	int size;
	char buf[0x1000];
	printf("ELF Length? \n");
	fflush(stdout);
	
	size = read_int();
	if((unsigned int)size > 0xA00000) {
		send_log("Too long size");
		exit(1);
	}

	printf("Data? \n");
	fflush(stdout);

	memset(&buf, 0, 0x1000uLL);
	int fd;
	if((fd = memfd_create("x", 1)) < 0) {
		send_log("Failed to memfd_create");
		exit(1);
	}
	while(size) {
		unsigned int tmp = 0x1000;
		if((unsigned int)size <= 0x1000)
			tmp = size;

		unsigned int read_size;
		if((read_size = read(0, &buf, tmp)) <= 0) {
			send_log("Read error");
			exit(1);
		}
		if(read_size != write(fd, &buf, read_size)) {
			send_log("Write error");
			exit(1);
		}
		size -= read_size;
	}
	return fd;
}
void sandbox()
{
	prctl(PR_SET_NO_NEW_PRIVS, 1);
	scmp_filter_ctx ctx;
	ctx = seccomp_init(SCMP_ACT_KILL); // init seccomp_filter
	if(ctx == NULL){
		printf("seccomp error\n");
		exit(-1);
	}
	seccomp_arch_add(ctx, SCMP_ARCH_X86_64);
	// read write open close socket execve
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
	//seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execveat), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fork), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(connect), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(uname), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readlink), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

	seccomp_load(ctx);

}

static int execveat(int fd, const char *path, char **argv, char **envp,
		     int flags)
{
#ifdef __NR_execveat
	return syscall(__NR_execveat, fd, path, argv, envp, flags);
#else
	return -1;
#endif
}
int main(int argc, char *argv[]) {
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stdout, 0, 2, 0); 
	setvbuf(stderr, 0, 2, 0);
	int fd = load_elf();

	int pid = fork();
	int status;

	char * your_argv[2] = {"bin", 0};
	char path = 0;

	if(pid > 0) {
		char buf[100];
		snprintf(buf, 100, "PID %d started", pid);
		send_log(buf);
		waitpid(pid, &status, 0);
	}
	else if(pid == 0) {
		if(unshare(CLONE_NEWNS) != 0) {
			perror("unshare");
			exit(1);
		}
		sleep(2);
		setuid(2019);
		sandbox();
		execveat(fd, &path, your_argv, 0, 0x1000);
	}
	else
		exit(1);
}
