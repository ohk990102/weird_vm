#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sched.h>
#include "socket.h"
#include "logger.h"
#include <vector>

int file_to_vector(const char *filename, std::vector<char *> &arr) {
	if (access(filename, R_OK) != 0)
		return 1;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	FILE *fp = fopen(filename, "re");
	if (fp == NULL)
		return 1;

	while ((read = getline(&line, &len, fp)) != -1) {
		// Remove end newline
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
		arr.push_back(line);
		line = NULL;
	}
	fclose(fp);
	return 0;
}

static int read_ns(const int pid, struct stat *st) {
	char path[32];
	sprintf(path, "/proc/%d/ns/mnt", pid);
	return stat(path, st);
}

static inline void lazy_unmount(const char* mountpoint) {
	umount2(mountpoint, MNT_DETACH);
}

int switch_mnt_ns(int pid) {
	char mnt[32];
	snprintf(mnt, sizeof(mnt), "/proc/%d/ns/mnt", pid);
	if(access(mnt, R_OK) == -1) return 1; // Maybe process died..

	int fd, ret;
	fd = open(mnt, O_RDONLY);
	if (fd < 0) return 1;
	// Switch to its namespace
	ret = setns(fd, 0);
	close(fd);
	return ret;
}
static int parse_ppid(int pid) {
	char path[32];
	int ppid;
	sprintf(path, "/proc/%d/stat", pid);
	FILE *stat = fopen(path, "re");
	if (stat == NULL)
		return -1;
	/* PID COMM STATE PPID ..... */
	fscanf(stat, "%*d %*s %*c %d", &ppid);
	fclose(stat);
	return ppid;
}
static void hide_daemon(int pid) {
	char buffer[4096];
	std::vector<char *> mounts;

	if (switch_mnt_ns(pid))
		goto exit;

	snprintf(buffer, sizeof(buffer), "/proc/%d", pid);
	chdir(buffer);

	file_to_vector("mounts", mounts);
	// Unmount all
	for (std::vector<char *>::iterator it = mounts.begin(); it != mounts.end(); ++it) {
		if(strstr(*it, "flag") == 0)
			continue;
		sscanf(*it, "%*s %4096s", buffer);
		lazy_unmount(buffer);
	}
	mounts.clear();


exit:
	// Send resume signal
	kill(pid, SIGCONT);
	_exit(0);
}

char msg[1024];
int main() {
	signal(SIGCHLD,SIG_IGN);
    while(1) {
        struct sockaddr_un sun;
        socklen_t len = setup_sockaddr(&sun, SOCKET_NAME);
        int fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
        connect(fd, (struct sockaddr *)&sun, len);
        send_int(fd, RECV_LOG);
        if(recv_int(fd) != DAEMON_OK)
            break;
        while(recv_line(fd, msg, 1024)) {
            int pid, ppid;
            struct stat ns, pns;
            if(sscanf(msg, "[VM]: PID %d started", &pid) == EOF)
                continue;
            if ((ppid = parse_ppid(pid)) < 0 || read_ns(ppid, &pns))
			    continue;
            while (read_ns(pid, &ns) == 0 && ns.st_dev == pns.st_dev && ns.st_ino == pns.st_ino)
			    usleep(500);
            if (kill(pid, SIGSTOP) == -1)
			    continue;
            int this_pid = fork();
            if(this_pid < 0)
                continue;
            else if(this_pid == 0)
                hide_daemon(pid);
        }
        close(fd);
    }
}
