
#include <stdio.h>
#include <string.h>
#include "logger.h"

int main() {
	char msg[100];
	fgets(msg, 100, stdin);
	char *pos;
	if ((pos=strchr(msg, '\n')) != NULL)
		    *pos = '\0';
	if(send_log(msg) < 0)
		return -1;
	return 0;
}
