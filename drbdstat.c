/* drbdstat 1.7
 * Simple program to continuously display DRBD status with adjustable update speed
 * (c)2012-2016 Miles Lott <mlott@gie.com>
 *
 * Compile with: gcc -o drbdstat drbdstat.c
 *
 * Although the function of this program can largely be duplicated simply by running, e.g.,
 * `watch cat /proc/drbdstat`, we add the ability to adjust the update speed of the display.
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * $Id: drbdstat.c,v 1.6 2016/02/25 19:52:27 gulfconfig Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	FILE *ptr_file, *ptr_version, *ptr_state, *ptr_ref;
	char ver[15];
	char ref[15];
	char state[15];
	char buf[1000];
	struct stat sbuf;
	char HOSTNAME[4096];
	int delay = 1;
	int detail = 0;

	gethostname(HOSTNAME, 4096);

	/* Fetch DRBD version */
	if(stat("/sys/module/drbd/version",&sbuf) > -1)
	{
		ptr_version = fopen("/sys/module/drbd/version","r");
	}
	else
	{
		fprintf(stderr,"Unable to open DRBD version file.  Is DRBD running?\n");
		exit(0);
	}
	while(fgets(ver,16, ptr_version)!=NULL)
	{
		printf("%s",ver);
	}
	fclose(ptr_version);
	/* chomp/trim */
	ver[strlen(ver) - 1] = '\0';

	while(1)
	{
		/* Fetch initstate, if available */
		if(stat("/sys/module/drbd/initstate",&sbuf) > -1)
		{
			ptr_state = fopen("/sys/module/drbd/initstate","r");
			while(fgets(state,16, ptr_state)!=NULL)
			{
				printf("%s",state);
			}
			fclose(ptr_state);
			state[strlen(state) - 1] = '\0';
		}
		else
		{
			strncpy(state,"unknown",7);
			fprintf(stderr,"Unable to open DRBD initstate file.  Is DRBD running?\n");
			exit(0);
		}

		/* Fetch refcnt */
		ptr_ref = fopen("/sys/module/drbd/refcnt","r");
		while(fgets(ref,16, ptr_ref)!=NULL)
		{
			printf("%s",ref);
		}
		fclose(ptr_ref);
		ref[strlen(ref) - 1] = '\0';

		/* Never less than one second delay */
		if(delay < 1)
		{
			delay = 1;
		}

		/* Read and print the status file */
		if(!(ptr_file = fopen("/proc/drbd","r")))
		{
			fprintf(stderr,"Unable to open DRBD process file.  Is DRBD running?\n");
			exit(0);
		}

		system("clear");
		fprintf(stderr,"DRBD %s status on %s, updated every %d second[s]\n",ver,HOSTNAME,delay);
		fprintf(stderr,"Init state: %s, Ref count: %s\n\n",state,ref);

		while(fgets(buf,1000, ptr_file)!=NULL)
		{
			printf("%s",buf);
		}
		fclose(ptr_file);

		fprintf(stderr,"\nPress [f] to speed up, [s] to slow down, [d] to toggle details, [q] or Ctrl-C to quit.\nHold any other key for near real-time update.\n");

		/* stty raw allows us to get a char without waiting for <enter> */
		system("stty raw");

		/* Now get a character */
		fd_set files;
		FD_ZERO(&files);
		FD_SET(0,&files);
		struct timeval timeout = { delay, 0 };
		int n = select(1,&files,NULL,NULL,&timeout);

		if(n > 0 && FD_ISSET(0,&files))
		{
			char ch = getchar();

			if(ch == 'd')
			{
				if(detail == 0)
				{
					detail = 1;
					system("echo 1 > /sys/module/drbd/parameters/proc_details");
				}
				else
				{
					detail = 0;
					system("echo 0 > /sys/module/drbd/parameters/proc_details");
				}
			}
			else if(ch == 'f')
			{
				delay = delay - 1;
			}
			else if(ch == 's')
			{
				delay = delay + 1;
			}
			else if(ch == 'q' || ch == 0x03)
			{
				system("stty cooked");
				exit(0);
			}

			fflush(stdout);
		}

		system("stty cooked");
	}
}
