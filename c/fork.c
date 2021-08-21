// fork.c shows the use of fork to create several child processes.
//
// build: cc fork.c -o fork
// use:   ./fork 5
//        ./fork 100000
//        meanwhile try ps u to show the current processes
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>	// printf
#include <stdlib.h>	// atoi exit EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork getpid sleep
#include <sys/wait.h>


// EXIT_SUCCESS or EXIT_FAILURE or an error number would be recommended.
// But in this trial I use the input value to show the relation
// between child and parent by the use of the exit status.
int child_process_function(const int seconds)
{
	printf("Child pid %d sleeps for %d seconds.\n", getpid(), seconds);
	sleep(seconds);
	printf("Child pid %d exits with %d.\n", getpid(), seconds);
	return seconds;
}


int main(const int argc, const char *argv[])
{
	int childs = 1;
	if (argc > 1)
		childs = atoi(argv[1]);

	// start the child processes
	for (int seconds = 1; seconds <= childs; ++seconds)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			// child
			const int status = child_process_function(seconds);
			return status;
			// anywhere else than main use exit(status);
			// to terminate the child process.
			// I prefer calling exit() close to fork() to be have the
			// obvious relation and not hidden in the called function.
		}

		// parent
		if (pid == -1)
			printf("Parent pid %d failed to create %d. child process."
				" Reason: %s\n", getpid(), seconds, strerror(errno));
		else
			printf("Parent pid %d created child %d.\n", getpid(), pid);
	}

	// parent waits for its child processes
	printf("Parent pid %d waits for its childs.\n", getpid());
	pid_t pid = 0;
	do
	{
		int status = 0;
		pid = wait(&status);
		// in the trial do not care about errno could be modified by printf
		printf("Parent pid %d wait pid %d status %d", getpid(), pid, status);
		if (pid != -1 && WIFEXITED(status))
			printf(" WEXITSTATUS %d", WEXITSTATUS(status));
		else if (pid == -1)
			printf(" errno %d (%s)", errno, strerror(errno));
		printf(".\n");
	} while (pid != -1 && errno != ECHILD);

	printf("Parent pid %d exits.\n", getpid());
	return EXIT_SUCCESS;
}
