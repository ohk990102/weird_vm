#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

static int execveat(int fd, const char *path, char **argv, char **envp,
		     int flags)
{
#ifdef __NR_execveat
	return syscall(__NR_execveat, fd, path, argv, envp, flags);
#else
	return -1;
#endif
}


int main() {
    int fd = open("test1", O_RDONLY);
    char path = 0;
    char * your_argv[2] = {"bin", 0};
    
    execveat(fd, &path, your_argv, 0, 0x1000);

}