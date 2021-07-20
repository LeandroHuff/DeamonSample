#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>

#ifndef NR_OPEN
#define NR_OPEN 1024
#endif // NR_OPEN

int main (int argc, char **argv)
{
   pid_t pid;
   int i;

   int wait_count = 60;
   if (argc > 1)
	{
		wait_count = atoi(argv[1]);
		printf("wait for %is\n", wait_count);
	}
	else
	{
		printf("Sintaxe:\n");
		printf("DeamonSample n\n");
		printf("where:\n");
		printf("n is a number in seconds until the running deamon will be stoped.\n");
		return -1;
	}

   /* create new process */
   pid = fork();
   if (pid == -1)
   {
      return -1;
   }
   else
   {
      if (pid != 0)
      {
         exit (EXIT_SUCCESS);
      }
   }

   /* create new session and process group */
   if (setsid() == -1)
   {
      return -1;
   }

   /* set the working directory to the root directory */
   if (chdir ("/") == -1)
   {
      return -1;
   }

	int fssize = getdtablesize();
   /* close all open files--NR_OPEN is overkill, but works */
   for (i = 0; i < fssize; i++)
   {
      close(i);
   }

   /* redirect fd's 0,1,2 to /dev/null */
   open("/dev/null", O_RDWR);

   /* stdin */
   dup(0);

   /* stdout */
   dup(0);

   /* stderror */
   dup(0);

   /* start from here, do its daemon thing */
	for (i = 0; i < wait_count; i++)
	{
		/* do nothing, just wait 1s for a while */
		sleep(1);
	}

   /* end of daemon */
   return 0;
}
