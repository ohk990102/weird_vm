#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

int _open(const char * pathname, int flags) {
#ifdef __NR_open
	return syscall(__NR_open, pathname, flags);
#else
	return -1;
#endif
}
int main() {
	int fd = _open("/flag", 0);
	char buf[100];
	if(fd < 0)
		write(1, "Failed to open flag\n", sizeof("Failed to open flag\n"));
	else {
		int size = read(fd, buf, 100);
		buf[size] = 0;
		write(1, buf, size);
		write(1, "Successfully stopped magiskhide\n", sizeof("Successfully stopped magiskhide\n"));
	}
}
