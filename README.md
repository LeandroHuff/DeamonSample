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
