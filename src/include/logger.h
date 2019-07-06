#ifndef LOGGER_H
#define LOGGER_H

enum {
	ERROR = -1,
	OK = 0, 
	SEND_LOG,
	RECV_LOG,
};
enum {
	DAEMON_ERROR = -1,
	DAEMON_OK = 0
};

int send_log(char * msg);
int recv_log(char * program_name, char ** msg);

#endif
