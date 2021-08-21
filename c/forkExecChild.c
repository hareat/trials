// forkExecChild.c sleeps for a while to show the use of fork+exec*
//                 to start several child programs.
//
// build: cc forkExecChild.c -o forkExecChild
// use:   ./forkExecChild
//        ./forkExecChild 3
/////////////////////////////////////////////////////////////////////

#include <stdio.h>	// printf
#include <stdlib.h>	// atoi
#include <unistd.h>	// getpid sleep


// EXIT_SUCCESS or EXIT_FAILURE or an error number would be recommended.
// But in this trial I use the input value to show the relation
// between child and parent by the use of the exit status.
int main(const int argc, const char *argv[])
{
	int seconds = 1;
	if (argc > 1)
		seconds = atoi(argv[1]);

	printf("Child pid %d sleeps for %d seconds.\n", getpid(), seconds);
	sleep(seconds);

	printf("Child pid %d exits with %d.\n", getpid(), seconds);
	return seconds;
}
