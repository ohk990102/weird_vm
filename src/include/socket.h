#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "WEIRD_SO_WEIRD"

#define ABS_SOCKET_LEN(sun) (sizeof(sun->sun_family) + strlen(sun->sun_path + 1) + 1)

socklen_t setup_sockaddr(struct sockaddr_un *sun, const char *name);
int recv_int(int fd);
void send_int(int fd, int val);
int recv_string(int fd, char **msg);
int send_string(int fd, char * msg);
int recv_line(int fd, char *msg, int n);
#endif
