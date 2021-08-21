// shm_sysv.c shows the use of System V shared memory.
// In System V a numeric key is the shared information all processes
// or binaries need. The necessary size is often calculated.
// Uses fork() to create multiple processes - see fork.c.
// In the trial do not care about closing ressources in the case of an error.
//
// build: cc shm_sysv.c -o shm_sysv
// use:   ./shm_sysv
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>	// EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork sleep
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


int child_process_shm_read(const key_t key, const size_t size)
{
	const int mode = 0444; 	// octal is common; read only
	const int shm_id = shmget(key, 0, mode);
	if (shm_id == -1)
	{
		fprintf(stderr, "%s: shmget(0x%x, %zu, 0%03o) failed with %d = %s\n",
			__func__, key, size, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s: shmget(0x%x, %zu, 0%03o) succeeded with shm_id = %d\n",
		__func__, key, size, mode, shm_id);

	char *addr = shmat(shm_id, NULL, SHM_RDONLY);
	if (addr == ((void*)-1))
	{
		fprintf(stderr, "shmat(%d, NULL, SHM_RDONLY) failed with %d = %s\n",
			shm_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	for (int i = 1; i <= 5; ++i)
	{
		printf("%s: %s\n", __func__, addr);
		sleep(1);
	}

	if (shmdt(addr) == -1)
	{
		fprintf(stderr, "%s: shmdt(%d) failed with %d = %s\n",
			__func__, shm_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int child_process_shm_write(const key_t key, const size_t size)
{
	const int mode = 0644; 	// octal is common
	const int shm_id = shmget(key, 0, mode);
	if (shm_id == -1)
	{
		fprintf(stderr, "%s: shmget(0x%x, %zu, 0%03o) failed with %d = %s\n",
			__func__, key, size, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s: shmget(0x%x, %zu, 0%03o) succeeded with shm_id = %d\n",
		__func__, key, size, mode, shm_id);

	char *addr = shmat(shm_id, NULL, 0);
	if (addr == ((void*)-1))
	{
		fprintf(stderr, "shmat(%d, NULL, 0) failed with %d = %s\n",
			shm_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	for (int i = 1; i <= 5; ++i)
	{
		// in the trial do not care about using a semaphore or a lock
		sprintf(addr, "child write %d", i);
		sleep(1);
	}

	if (shmdt(addr) == -1)
	{
		fprintf(stderr, "%s: shmdt(%d) failed with %d = %s\n",
			__func__, shm_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
	const key_t key = 0x1234;	// hex is common
	const size_t size = 1234;
	const int mode = 0644;   	// octal is common

	// IPC_EXCL | IPC_CREAT should ensure shm is created by this process.
	const int shm_id = shmget(key, size, mode | IPC_EXCL | IPC_CREAT);
	if (shm_id == -1)
	{
		fprintf(stderr, "shmget(0x%x, %zu, 0%03o | IPC_EXCL | IPC_CREAT)"
			" failed with %d = %s\n", key, size, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s; shmget(0x%x, %zu, 0%03o | IPC_EXCL | IPC_CREAT) succeeded"
		" with shm_id = %d\n", __func__, key, size, mode, shm_id);
	printf("You can delete the shared memory with: ipcrm -m %d\n", shm_id);

	char *addr = shmat(shm_id, NULL, 0);
	if (addr == ((void*)-1))
	{
		fprintf(stderr, "shmat(%d, NULL, 0) failed with %d = %s\n",
			shm_id, errno, strerror(errno));
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
		return child_process_shm_read(key, size);
	// parent

	pid = fork();
	if (pid == -1)
	{
		printf("Parent failed to create %s. Reason: %s\n",
			"child_process_shm_write", strerror(errno));
		return EXIT_FAILURE;
	}
	if (pid == 0)		// child
		return child_process_shm_write(key, size);
	// parent

	// wait for all child processes to terminate
	do {
		int status = 0;
		pid = wait(&status);
	} while (pid != -1 && errno != ECHILD);

	printf("%s: %s\n", __func__, addr);
	if (shmdt(addr) == -1)
	{
		fprintf(stderr, "shmdt(%p) failed with %d = %s\n",
			addr, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	// remove the shared memory
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		fprintf(stderr, "shmctl(%d, IPC_RMID, NULL) failed with %d = %s\n",
			shm_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
