// signal.c shows the use of signal to handle signals.
// use kill -l for a list of signal numbers
//
// build: cc signal.c -o signal
// use:   ./signal
//            you have to kill with killall -9 signal
//            because CTRL+C is handled by the signal handler, try it
//        ./signal 10 12
//            killall -s SIGUSR1 signal
//            killall -s SIGUSR2 signal
//          CTRL+C
//        ./signal 10
//            killall -s SIGUSR1 signal
//            killall -s SIGUSR2 signal
/////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <stdio.h>	// perror
#include <stdlib.h>	// atoi EXIT*
#include <string.h>	// strlen
#include <unistd.h>	// sleep write


const char* signum_to_sz(const int signum)
{
	switch (signum)
	{
		case SIGHUP:
			return "SIGHUP\n";
		case SIGINT:
			return "SIGINT\n";
		case SIGQUIT:
			return "SIGQUIT\n";
		case SIGILL:
			return "SIGILL\n";
		case SIGTRAP:
			return "SIGTRAP\n";
		case SIGABRT:
			return "SIGABRT\n";
		case SIGBUS:
			return "SIGBUS\n";
		case SIGFPE:
			return "SIGFPE\n";
		case SIGKILL:
			return "SIGKILL\n";
		case SIGUSR1:
			return "SIGUSR1\n";
		case SIGSEGV:
			return "SIGSEGV\n";
		case SIGUSR2:
			return "SIGUSR2\n";
		case SIGPIPE:
			return "SIGPIPE\n";
		case SIGALRM:
			return "SIGALRM\n";
		case SIGTERM:
			return "SIGTERM\n";
		case SIGCHLD:
			return "SIGCHLD\n";
		case SIGCONT:
			return "SIGCONT\n";
		case SIGSTOP:
			return "SIGSTOP\n";
		case SIGTSTP:
			return "SIGTSTP\n";
		case SIGVTALRM:
			return "SIGVTALRM\n";

		default:
			return "unknown\n";
	}
}


void signal_handler(int signum)
{
	const char *signame = signum_to_sz(signum);
	// use write because it is signal safe. see man signal-safety
	write(STDOUT_FILENO, signame, strlen(signame));
}


int main(const int argc, const char* argv[])
{
	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			if (signal(atoi(argv[i]), signal_handler) == SIG_ERR) {
				perror(argv[i]);
				return EXIT_FAILURE;
			}
	} else {	// no args
		if (signal(SIGINT,  signal_handler) == SIG_ERR
		 || signal(SIGQUIT, signal_handler) == SIG_ERR
		 || signal(SIGABRT, signal_handler) == SIG_ERR
		 || signal(SIGKILL, signal_handler) == SIG_ERR
		 || signal(SIGUSR1, signal_handler) == SIG_ERR
		 || signal(SIGUSR2, signal_handler) == SIG_ERR
		 || signal(SIGALRM, signal_handler) == SIG_ERR
		 || signal(SIGTERM, signal_handler) == SIG_ERR
		 || signal(SIGCHLD, signal_handler) == SIG_ERR
		 || signal(SIGCONT, signal_handler) == SIG_ERR
		 || signal(SIGSTOP, signal_handler) == SIG_ERR
		 || signal(SIGTSTP, signal_handler) == SIG_ERR) {
			perror("");
			return EXIT_FAILURE;
		}
	}

	for (int seconds = 1;; ++seconds) {
		const char *output = "sleeping\n";
		write(STDOUT_FILENO, output, strlen(output));
		sleep(seconds);
	}

	return EXIT_SUCCESS;
}
