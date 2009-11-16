/* 
    kleds for KDE2/KDE3

    Copyright (C) 2001,2002 by Hans Matzen (hans@hansmatzen.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
    
    // make me a daemon
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
	printf("Error in fork\n");
	return(-1); //  error forking
    }
    else if (pid != 0) 
	exit(0); // parent die
    
    // child continues
    if (setpgrp() == -1)
	printf("error whil setprgp\n");
    
    //   setsid();
    // make sure not to be on a mounted filesystem
    chdir("/");
    // clear any inherited file mode mask
    umask(0);
    
    xkbwatch *xkbw = new xkbwatch((char *) disp);
    xkbw->run();
    
    return 0;
}


