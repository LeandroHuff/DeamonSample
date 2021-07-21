How to Create a Daemon Service in Linux   


In a Unix environment, the parent process of a daemon is often, but not always, the init process. A daemon is usually created either by a process forking a child process and then immediately exiting, thus causing init to adopt the child process, or by the init process directly launching the daemon.


This involves a few steps:
1. Fork off the parent process.
2. Change file mode mask (umask)
3. Open any logs for writing.
4. Create a unique Session ID (SID)
5. Change the current working directory to a safe place.
6. Close standard file descriptors.
7. Enter actual daemon code.


Deamon - Run in the Background

The daemon() function is for programs wishing to detach themselves from the controlling terminal and run in the background as system daemons.
If nochdir is zero, daemon() changes the process's current working directory to the root directory ("/"); otherwise, the current working directory is left unchanged.
If noclose is zero, daemon() redirects standard input, standard output, and standard error to /dev/null; otherwise, no changes are made to these file descriptors.


Here are the steps to become a daemon:

1. fork() so the parent can exit, this returns control to the command line or shell invoking your program. This step is required so that the new process is guaranteed not to be a process group leader. The next step, setsid(), fails if you're a process group leader.
2. setsid() to become a process group and session group leader. Since a controlling terminal is associated with a session, and this new session has not yet acquired a controlling terminal our process now has no controlling terminal, which is a Good Thing for daemons.
3. fork() again so the parent, (the session group leader), can exit. This means that we, as a non-session group leader, can never regain a controlling terminal.
4. chdir("/") to ensure that our process doesn't keep any directory in use. Failure to do this could make it so that an administrator couldn't unmount a filesystem, because it was our current directory. [Equivalently, we could change to any directory containing files important to the daemon's operation.]
5. umask(0) so that we have complete control over the permissions of anything we write. We don't know what umask we may have inherited. [This step is optional]
6. close() fds 0, 1, and 2. This releases the standard in, out, and error we inherited from our parent process. We have no way of knowing where these fds might have been redirected to. Note that many daemons use sysconf() to determine the limit _SC_OPEN_MAX. _SC_OPEN_MAX tells you the maximun open files/process. Then in a loop, the daemon can close all possible file descriptors. You have to decide if you need to do this or not. If you think that there might be file-descriptors open you should close them, since there's a limit on number of concurrent file descriptors.
7. Establish new open descriptors for stdin, stdout and stderr. Even if you don't plan to use them, it is still a good idea to have them open. The precise handling of these is a matter of taste; if you have a logfile, for example, you might wish to open it as stdout or stderr, and open '/dev/null' as stdin; alternatively, you could open '/dev/console' as stderr and/or stdout, and '/dev/null' as stdin, or any other combination that makes sense for your particular daemon.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>

int main (void)
{
   pid_t pid;
   int i;

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

   /* close all open files--NR_OPEN is overkill, but works */
   for (i = 0; i < NR_OPEN; i++)
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


   /* end of daemon */
   return 0;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Daemon is called as a type of program which quietly runs in the background rather than under the direct control of a user. It means that a daemon does not interact with the user.

Systemd
Management of daemons is done using systemd. It is a system and service manager for Linux operating systems. It is designed to be backwards compatible with SysV init scripts, and provides a number of features such as parallel startup of system services at boot time, on-demand activation of daemons, or dependency-based service control logic.

Units
Systemd introduced us with the concept of systemd units. These units are represented by unit configuration files located in one of the directories listed below:

| Directory | Description |
| /usr/lib/systemd/system/ | Systemd unit files distributed with installed RPM packages. |
| /run/systemd/system/ | Systemd unit files created at run time. This directory takes precedence over the directory with installed service unit files. |
| /etc/systemd/system/ | Systemd unit files created by systemctl enable as well as unit files added for extending a service. This directory takes precedence over the directory with runtime unit files. |


We have multiple unit types available to us. The below table gives a brief description for each of them.

| Unit Type | File Extension | Description |
| Service unit | .service | A system service. |
| Target unit | .target | A group of systemd units. |
| Automount unit | .automount | A file system automount point. |
| Device unit | .device | A device file recognized by the kernel. |
| Mount unit | .mount | A file system mount point. |
| Path unit | .path | A file or directory in a file system. |
| Scope unit | .scope | An externally created process. |
| Slice unit | .slice | A group of hierarchically organized units that manage system processes. |
| Snapshot unit | .snapshot | A saved state of the systemd manager. |
| Socket unit | .socket | An inter-process communication socket. |
| Swap unit | .swap | A swap device or a swap file. |
| Timer unit | .timer | A systemd timer. |



Creating Our Own Daemon
At many times we will want to create our own services for different purposes. For this blog, we will be using a Java application, packaged as a jar file and then we will make it run as a service.


Step 1: JAR File
The first step is to acquire a jar file. We have used a jar file which has implemented a few routes in it.


Step 2: Script
Secondly, we will be creating a bash script that will be running our jar file. Note that there is no problem in using the jar file directly in the unit file, but it is considered a good practice to call it from a script. It is also recommended to store our jar files and bash script in /usr/bin directory even though we can use it from any location on our systems.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#!/bin/bash
/usr/bin/java -jar <name-of-jar-file>.jar
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Make sure that you make this script executable before running it:


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
chmod +x <script-name>.sh
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



Step 3: Units File
Now that we have created an executable script, we will be using it into make our service.
We have here a very basic .service unit file.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[Unit]
Description=A Simple Java Service

[Service]
WorkingDirectory=/usr/bin
ExecStart= /bin/bash /usr/bin/java-app.sh
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


In this file, the Description tag is used to give some detail about our service when someone will want to see the status of the service.
The WorkingDirectory is used to give path of our executables.
ExecStart tag is used to execute the command when we start the service.
The Restart tag configures whether the service shall be restarted when the service process exits, is killed, or a timeout is reached.
multi-user.target normally defines a system state where all network services are started up and the system will accept logins, but a local GUI is not started. This is the typical default system state for server systems, which might be rack-mounted headless systems in a remote server room.


Step 4: Starting Our Daemon Service
Let us now look at the commands which we will use to run our custom daemon.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sudo systemctl daemon-reload
# Uncomment the below line to start your service at the time of system boot
# sudo systemctl enable <name-of-service>.service
sudo systemctl start <name-of-service>
# OR
# sudo service <name-of-service> start
sudo systemctl status <name-of-service>
# OR
# sudo service <name-of-service> status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



Conclusion
In this blog, we have looked how to make custom daemons and check their status as well. Also, we observed that it is fairly easy to make these daemons and use them. We hope that everyone is now comfortable enough to make daemons on their own.


References:<\b>
https://dzone.com/articles/run-your-java-application-as-a-service-on-ubuntu
https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/system_administrators_guide/chap-managing_services_with_systemd



# DeamonSample.c

This project is a simple Linux Deamon demonstration with a minimal software code to demonstrate and run the process for a while in seconds until finish it.
The time in seconds to wait until finish is passed from command line as an argument to the DeamonSample software.
The deamon process wont do anything, just sleep for 1s while decrement a counter wainting to reach zero, for each interation, the deamon software sleep and at the end of counter time the deamon stop and return.
The deamon running could be viewed by htop linux tool and filtered by name pressing F4 to enter "DeamonSample" name.

Sintaxe:
./DeamonSample 10

Start the DeamonSample and wait for 10 seconds until to stop itself.

To watch the DeamonSample running, execute htop linux command as:

htop

Inside the program, hit F4 and write DeamonSample and press Enter to filter all process by this name.
The htop process list will be reduced to only one process with name DeamonSample, if it is running.
After time elapsed the process is stoped and the htop remove the DeamonSample program from the list to show that the process was finished.

This example can be improved according to your needs and the application of your deamon process, be free to use it as your convenience.
I hope you enjoy it and this example can be useful to your self development and application.
