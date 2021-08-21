// forkExec.c shows the use of fork+exec* to start several child programs.
//
// build: cc forkExec.c -o forkExec
// use:   ./forkExec 3 
//        ./forkExec 5 /bin/sleep
//        meanwhile try ps u to show the current processes
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>	// printf
#include <stdlib.h>	// atoi exit EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork getpid sleep
#include <sys/wait.h>


int main(const int argc, const char *argv[])
{
	int childs = 1;
	if (argc > 1)
		childs = atoi(argv[1]);

	char *childbin = NULL;
	if (argc > 2)
		childbin = strdup(argv[2]);
	else
		childbin = strdup("bin/forkExecChild");
	// in the trial do not care about childbin == NULL

	// start the child processes
	char buffer[22] = "";
	for (int seconds = 1; seconds <= childs; ++seconds)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			// child
			snprintf(buffer, 22, "%d", seconds);
			char* argvChild[3] = {childbin, buffer};
			execv(childbin, argvChild);
			// exec only return in case of an error
			printf("Parent pid %d failed to replace %d. process image."
				" Reason: %s\n", getpid(), seconds, strerror(errno));
			return EXIT_FAILURE;
			// anywhere else than main use exit(EXIT_FAILURE);
			// to terminate the child process.
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

	free(childbin);
	printf("Parent pid %d exits.\n", getpid());
	return EXIT_SUCCESS;
}
