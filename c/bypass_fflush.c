// bypass_fflush.c Are file buffers flushed in case of forgotten fclose,
//                 a terminating signal or an exit call?
//     Answer: no  in case of an abnormal program end by a terminating signal
//                 or an _exit call.
//             yes in case of a normal program end by reaching end of life
//                 or an exit call.
//
// build: cc bypass_fflush.cpp -o bypass_fflush
// use:   ./bypass_fflush ; cat tmp/f?
//             unbuffered file content
//             buffered file content
//        ./bypass_fflush term ; cat tmp/f?
//             raise SIGTERM
//             Terminated
//             unbuffered file content
//        ./bypass_fflush exit ; cat tmp/f?
//             call stdlib exit
//             unbuffered file content
//             buffered file content
//        ./bypass_fflush _exit ; cat tmp/f?
//             call unistd _exit
//             unbuffered file content
/////////////////////////////////////////////////////////////////////

#include <fcntl.h>	// open
#include <signal.h>	// raise SIG*
#include <stdio.h>	// fputs perror
#include <stdlib.h>	// exit EXIT*
#include <string.h>	// strcmp strlen
#include <unistd.h>	// _exit unlink write


int main(const int argc, const char* argv[])
{
	// open the FILE pointer
	const char *filename = "tmp/fp";
	unlink(filename);
	FILE *fp = fopen(filename, "w");
	if (!fp) {
		perror(filename);
		return EXIT_FAILURE;
	}

	// open the file handle
	filename = "tmp/fh";
	unlink(filename);
	int fh = open(filename, O_WRONLY | O_CREAT, 0644);
	if (fh == -1) {
		perror(filename);
		return EXIT_FAILURE;
	}

	// fill the files
	fputs("buffered file content\n", fp);
	const char * const content = "unbuffered file content\n";
	write(fh, content, strlen(content));

	// decide which kind of process end you want
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

	// in this trial do not fclose(fp) and close(fh)

	return EXIT_SUCCESS;
}
