/*
 * Based on kleds-0.8.0, written by Hans Matzen (hans@hansmatzen.de)
 */
//
// main program for kleds daemon (kledsd)
//

#define VERSION "0.6.1"

extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
}

#include "xkbwatch.h"

int main(int argc,char **argv)
{

	// get DISPLAY env var
	const void * disp=getenv("DISPLAY");

	// if DISPLAY is not set try the default
	if (!disp)
		disp = ":0.0";

#if 0
	// make me a daemon
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		printf("Error in fork\n");
		return(-1); // error forking
	}
	else if (pid != 0)
		exit(0); // parent die

	// child continues
	if (setpgrp() == -1)
		printf("error whil setprgp\n");

	// setsid();
	// make sure not to be on a mounted filesystem
	chdir("/");
	// clear any inherited file mode mask
	umask(0);
#endif

	xkbwatch *xkbw = new xkbwatch((char *) disp);
	xkbw->run();

	return 0;
}


