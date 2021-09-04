// bypass_destructor.cpp Are destructors processed in case of a
//                       terminating signal or an exit call?
//      expected Answer: no
//
// build: cc bypass_destructor.cpp -o bypass_destructor
// use:   ./bypass_destructor ; echo $?
//             Notifier
//             ~Notifier
//             0
//        ./bypass_destructor term ; echo $?
//             Notifier
//             raise SIGTERM
//             Terminated
//             143
//        ./bypass_destructor exit ; echo $?
//             Notifier
//             call stdlib exit
//             1
//        ./bypass_destructor _exit ; echo $?
//             Notifier
//             call unistd _exit
//             1
/////////////////////////////////////////////////////////////////////

#include <signal.h>	// raise SIG*
#include <stdio.h>	// puts
#include <stdlib.h>	// exit EXIT*
#include <string.h>	// strcmp
#include <unistd.h>	// _exit


struct Notifier
{
	Notifier()  { puts(__func__); }
	~Notifier() { puts(__func__); }
};


int main(const int argc, const char* argv[])
{
	Notifier notifier;
	if (argc == 2) {
		if (strcmp(argv[1], "term") == 0) {
			puts("raise SIGTERM");
			raise(SIGTERM);
		} else if (strcmp(argv[1], "exit") == 0) {
			puts("call stdlib exit");
			exit(EXIT_FAILURE);
		} else if (strcmp(argv[1], "_exit") == 0) {
			puts("call unistd _exit");
			_exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
