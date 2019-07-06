#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "logger.h"

int _fork() {
#ifdef __NR_fork
	return syscall(__NR_fork);
#else
	return -1;
#endif
}
int main() {
	write(1, "1\n", 2);
	int pid = _fork();
	if(pid < 0) {
		printf("fork\n");
	}
	else if(pid == 0) {
		while(1);
	}
	else {
		write(1, "2\n", 2);
		char buf[100];
		snprintf(buf, 100, "abcd\n[VM]: PID %d started", pid);
		send_log(buf);
		write(1, "3\n", 2);
	}
}
