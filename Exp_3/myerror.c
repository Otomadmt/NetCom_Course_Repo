#ifndef _MY_ERROR_
#define _MY_ERROR_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void
err_fnc(const char *func) {
	char *mesg = strerror(errno);
   	printf("[ERROR]%s(): %s\n", func, mesg);
   	exit(1);
}

void
err_wrn(const char *func) {
	char *mesg = strerror(errno);
   	printf("[WARN]%s(): %s\n", func, mesg);
}

#endif 
