// shm_posix.c shows the use of POSIX shared memory.
// In POSIX a pathname relative to /dev/shm is the shared information
// all processes or binaries need. The necessary size is often calculated.
// Uses fork() to create multiple processes - see fork.c.
// In the trial do not care about closing ressources in the case of an error.
//
// build: cc shm_posix.c -o shm_posix
// use:   ./shm_posix
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <fcntl.h>	// O_*
#include <stdio.h>
#include <stdlib.h>	// EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork sleep
#include <sys/mman.h>
#include <sys/wait.h>


int child_process_shm_read(const char *name, const size_t size)
{
	const int mode = 0444; 	// octal is common; read only
	const int shm_fd = shm_open(name, O_RDONLY, mode);
	if (shm_fd == -1)
	{
		fprintf(stderr, "shm_open(%s, O_RDONLY, 0%03o)"
			" failed with %d = %s\n", name,  mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("shm_open(%s, O_RDONLY, 0%03o) succeeded"
		" with shm_fd = %d\n", name, mode, shm_fd);

	char *addr = mmap(NULL, size, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (addr == MAP_FAILED)
	{
		fprintf(stderr, "%s: %s failed with %d = %s\n",
			__func__, "mmap", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	for (int i = 1; i <= 5; ++i)
	{
		printf("child read: %s\n", addr);
		sleep(1);
	}

	if (close(shm_fd) == -1)
	{
		fprintf(stderr, "%s: %s(%d) failed with %d = %s\n",
			__func__, "shmdt", shm_fd, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int child_process_shm_write(const char *name, const size_t size)
{
	const int mode = 0644; 	// octal is common
	const int shm_fd = shm_open(name, O_RDWR, mode);
	if (shm_fd == -1)
	{
		fprintf(stderr, "shm_open(%s, O_RDWR, 0%03o)"
			" failed with %d = %s\n", name,  mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("shm_open(%s, O_RDRW, 0%03o) succeeded"
		" with shm_fd = %d\n", name, mode, shm_fd);

	char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (addr == MAP_FAILED)
	{
		fprintf(stderr, "%s: %s failed with %d = %s\n",
			__func__, "mmap", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	for (int i = 1; i <= 5; ++i)
	{
		// in the trial do not care about using a semaphore or a lock
		sprintf(addr, "child write %d\n", i);
		sleep(1);
	}

	if (close(shm_fd) == -1)
	{
		fprintf(stderr, "%s: %s(%d) failed with %d = %s\n",
			__func__, "shmdt", shm_fd, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
	const char *name = "/shm_posix";
	const size_t size = 1234;
	const int mode = 0644;   	// octal is common

	// IPC_EXCL | IPC_CREAT should ensure shm is created by this process.
	const int shm_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, mode);
	if (shm_fd == -1)
	{
		fprintf(stderr, "shm_open(%s, O_RDWR | O_CREAT | O_EXCL, 0%03o)"
			" failed with %d = %s\n", name,  mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("shm_open(%s, O_RDWR | O_CREAT | O_EXCL, 0%03o) succeeded"
		" with shm_fd = %d\n", name, mode, shm_fd);
	printf("You can delete the shared memory with: rm /dev/shm%s\n", name);
	
	// without fallocate you will get a "bus error"
	if (posix_fallocate(shm_fd, 0, size) == -1)
	{
		fprintf(stderr, "%s failed with %d = %s\n",
			"fallocate", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (addr == MAP_FAILED)
	{
		fprintf(stderr, "%s failed with %d = %s\n",
			"mmap", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	strcpy(addr, "filled by parent");

	pid_t pid = fork();
	if (pid == -1)
	{
		printf("Parent failed to create %s. Reason: %s\n",
			"child_process_shm_read", strerror(errno));
		return EXIT_FAILURE;
	}
	if (pid == 0)		// child
		return child_process_shm_read(name, size);
	// parent

	pid = fork();
	if (pid == -1)
	{
		printf("Parent failed to create %s. Reason: %s\n",
			"child_process_shm_write", strerror(errno));
		return EXIT_FAILURE;
	}
	if (pid == 0)		// child
		return child_process_shm_write(name, size);
	// parent

	// wait for all child processes to terminate
	do {
		int status = 0;
		pid = wait(&status);
	} while (pid != -1 && errno != ECHILD);

	printf("parent read: %s\n", addr);
	if (munmap(addr, size) == -1)
	{
		fprintf(stderr, "munmap(%p) failed with %d = %s\n",
			addr, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	if (close(shm_fd) == -1)
	{
		fprintf(stderr, "%s: %s(%d) failed with %d = %s\n",
			__func__, "shmdt", shm_fd, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	// remove the shared memory
	if (shm_unlink(name) == -1)
	{
		fprintf(stderr, "shm_unlink(%s) failed with %d = %s\n",
			name, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
