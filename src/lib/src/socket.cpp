#include "socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "WEIRD_SO_WEIRD"
#define ABS_SOCKET_LEN(sun) (sizeof(sun->sun_family) + strlen(sun->sun_path + 1) + 1)
#define MAX_MSG_LENGTH 1024

socklen_t setup_sockaddr(struct sockaddr_un *sun, const char *name) {
    memset(sun, 0, sizeof(*sun));
    sun->sun_family = AF_LOCAL;
	strcpy(sun->sun_path + 1, name);
    return ABS_SOCKET_LEN(sun);
}
int recv_int(int fd) {
	int val;
	if(read(fd, &val, sizeof(val)) != sizeof(val))
		return -1;
	return val;
}
void send_int(int fd, int val) {
	if(fd < 0) return;
	write(fd, &val, sizeof(val));
}

static char *recv_str(int fd, int len) {
	char *val = (char *) malloc(sizeof(char) * (len + 1));
	if(read(fd, val, len) != len)
		return NULL;
	val[len] = '\x00';
	return val;
}

int recv_string(int fd, char ** msg) {
	int len = recv_int(fd);
	if(len <= 0 || len > MAX_MSG_LENGTH) {
		return -1;
	}
	char * tmp;
	if((tmp = recv_str(fd, len)) == NULL)
		return -1;
	*msg = tmp;
	return len;
}
int send_string(int fd, char * msg) {
	int len = strlen(msg);
	send_int(fd, len);
	if(write(fd, msg, len) != len)
		return -1;
	return 0;
}
int recv_line(int fd, char *buffer, int n)
{
    int numRead; 
    int totRead;                     
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL)
        return -1;
    buf = buffer; 

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);
        if (numRead == -1) {
            return -1;
        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;
        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }
            if (ch == '\n')
                break;
        }
    }
    *buf = '\0';
    return totRead;
}