#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "socket.h"
#include "logger.h"
#include <vector>

#define SHELLCODE_LENGTH 1000

using namespace std;
void welcome() {
	puts("===============================");
	puts("");
	puts("Welcome to weirdVM");
	puts("Enter ELF in base64 format");
	puts("");
	puts("===============================");
}

char shellcode[SHELLCODE_LENGTH];
vector<int> response_fds;
int executor_pid = -1;
int mounthide_pid = -1;
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER; // executor_pid, mounthide_pid
pthread_mutex_t b_mutex = PTHREAD_MUTEX_INITIALIZER; // response_fds

static void get_client_cred(int fd, struct ucred *cred) {
	socklen_t ucred_length = sizeof(*cred);
	if(getsockopt(fd, SOL_SOCKET, SO_PEERCRED, cred, &ucred_length))
		perror("getsockopt");
}

void recv_client_log(int fd, const char * program_name) {
	char * msg;
	if(recv_string(fd, &msg) == -1) {
		send_int(fd, DAEMON_ERROR);
		printf("recv_string\n");
		return;
	}
	send_int(fd, DAEMON_OK);

	int len = strlen(msg);
	len += strlen(program_name);
	len += 10;

	char * real_msg = (char *) malloc(sizeof(char) * len);
	snprintf(real_msg, len, "[%s]: %s\n", program_name, msg);
	printf("%s", real_msg);

	pthread_mutex_lock(&b_mutex);
	for(vector<int>::iterator it = response_fds.begin(); it != response_fds.end(); it++) {
		if(write(*it, real_msg, strlen(real_msg)) < 0)
			it = response_fds.erase(it);
	}
	pthread_mutex_unlock(&b_mutex);
}
void *request_handler(void *args) {
	int client = *((int *) args);

	struct ucred credential;
	get_client_cred(client, &credential);

	const char *program_name = "Shellcode";
	pthread_mutex_lock(&a_mutex);
	if(credential.pid == executor_pid)
		program_name = "VM";
	if(credential.pid == mounthide_pid)
		program_name = "MountHide";
	pthread_mutex_unlock(&a_mutex);
	int req = recv_int(client);
	switch(req) {
	case SEND_LOG:
		send_int(client, DAEMON_OK);
		recv_client_log(client, program_name);
		break;
	case RECV_LOG:
		send_int(client, DAEMON_OK);
		pthread_mutex_lock(&b_mutex);
		response_fds.push_back(client);
		pthread_mutex_unlock(&b_mutex);
		return NULL;
	default:
		send_int(client, DAEMON_ERROR);
		break;
	}
	close(client);
	return NULL;
}

void *logcat_daemon(void *args) {
	
	int fd = *((int*) args);

	while(1) {
		int *client = (int *) malloc(sizeof(int));
		*client = accept(fd, NULL, NULL);
		if(*client == -1) {
			perror("accept");
			continue;
		}
		fcntl(*client, F_SETFD, FD_CLOEXEC);
		pthread_t thread;
		pthread_create(&thread, NULL, request_handler, client);
		pthread_detach(thread);
	}
}

int prepare() {
	// ignore SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	
	// Run Mounthide Daemon with fork / exec
	// should lock before updating pid

	int pid = fork();

	if(pid < 0) {
		perror("fork");
		exit(1);
	}
	else if(pid == 0) {
		execl("mounthide", "mounthide", NULL);
	}
	pthread_mutex_lock(&a_mutex);
	mounthide_pid = pid;
	pthread_mutex_unlock(&a_mutex);

	pthread_t logcat_daemon_thread;
	struct sockaddr_un sun;
	socklen_t len = setup_sockaddr(&sun, SOCKET_NAME);
	int *fd = (int *) malloc(sizeof(int));
	*fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
	
	if(bind(*fd, (struct sockaddr *) &sun, len)) {
		perror("bind");
		exit(1);
	}
	listen(*fd, 10);
	pthread_create(&logcat_daemon_thread, NULL, logcat_daemon, fd);
}
int main(int argc, char *argv[]) {
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stdout, 0, 2, 0);
	setvbuf(stderr, 0, 2, 0);
	
	prepare();
	welcome();
	
	while(1) {
		ssize_t size;
		int pid = fork();
		int wstatus;
		if(pid < 0) {
			perror("fork");
			exit(1);
		}
		else if(pid == 0) {
			execl("executor", "executor", NULL);
			exit(1);
		}
		else {
			pthread_mutex_lock(&a_mutex);
			executor_pid = pid;
			pthread_mutex_unlock(&a_mutex);
			waitpid(pid, &wstatus, 0);
		}
	}
}
