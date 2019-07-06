#include "logger.h"
#include "socket.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int send_log(char * msg) {
	struct sockaddr_un sun;
	socklen_t len = setup_sockaddr(&sun, SOCKET_NAME);

	int fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
	
	if(connect(fd, (struct sockaddr *)&sun, len) == 0) {
		send_int(fd, SEND_LOG);
		int ret = recv_int(fd);
		if(ret != DAEMON_OK)
			goto ERROR;
		send_string(fd, msg);
		ret = recv_int(fd);
		if(ret != DAEMON_OK)
			goto ERROR;
	}
	else 
		goto ERROR_2;

SUCCESS:
	close(fd);
	return 0;
ERROR:
	close(fd);
ERROR_2:
	return -1;
}

