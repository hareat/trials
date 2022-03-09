// sem_sysv_writeonly.c shows the use of System V semaphores.
// Create three child processes which want an exclusive access to a shared
// resource and therefore use semval = 0 as sign for exclusive access.
// In System V a numeric key is the shared information all processes
// or binaries need. If you want to wait and lock ensure that this
// is an atomic operation (semop() with two sembufs).
// Uses fork() to create multiple processes - see fork.c.
// In the trial do not care about removeing system resources
// in the case of an error.
//
// build: cc sem_sysv_writeonly.c -o sem_sysv_writeonly
// use:   ./sem_sysv_writeonly
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>	// EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork sleep
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define LOCK    1
#define UNLOCK -1
#define WAIT    0

int child_process(const key_t key, const unsigned int seconds)
{
	const int pid = getpid();
	const unsigned int indention = 2 * seconds;
	const int nsems = 0;	// don't care because we depend on an existing one
	const int mode = 0; 	// don't care because we depend on an existing one
	const int sem_id = semget(key, nsems, mode);
	if (sem_id == -1)
	{
		fprintf(stderr, "%s(%u): semget(0x%x, %d, 0%03o) failed with %d = %s\n",
			__func__, seconds, key, nsems, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s child(%u) pid=%d: semget(0x%x, %d, 0%03o) succeeded with sem_id = %d\n",
		indention, "", seconds, pid, key, nsems, mode, sem_id);

	printf("%*s child(%u) pid=%d: GETNCNT=%d GETPID=%d GETVAL=%d GETZCNT=%d\n",
		indention, "", seconds, pid,
		semctl(sem_id, 0, GETNCNT), semctl(sem_id, 0, GETPID),
		semctl(sem_id, 0, GETVAL), semctl(sem_id, 0, GETZCNT));

	struct sembuf buf2[2] = {0, WAIT, SEM_UNDO, 0, LOCK, SEM_UNDO};
	if (semop(sem_id, buf2, 2) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "wait-for-zero and lock", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s child(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", seconds, pid, "wait-for-zero and lock");

	for (unsigned int i = seconds; i; --i)
	{
		printf("%*s child(%u) pid=%d: sleeps %d seconds\n", indention, "", seconds, pid, i);
		sleep(1);
	}

	struct sembuf buf = {0, UNLOCK, SEM_UNDO};
	if (semop(sem_id, &buf, 1) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "unlock", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s child(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", seconds, pid, "unlock");

	printf("%*s child(%u) pid=%d: GETNCNT=%d GETPID=%d GETVAL=%d GETZCNT=%d\n",
		indention, "", seconds, pid,
		semctl(sem_id, 0, GETNCNT), semctl(sem_id, 0, GETPID),
		semctl(sem_id, 0, GETVAL), semctl(sem_id, 0, GETZCNT));

	return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
	const key_t key = 0x1234;	// hex is common
	const int nsems = 1;
	const int mode = 0644;   	// octal is common

	// IPC_EXCL | IPC_CREAT should ensure sem is created by this process.
	const int sem_id = semget(key, nsems, mode | IPC_CREAT);
	if (sem_id == -1)
	{
		fprintf(stderr, "semget(0x%x, %d, 0%03o | IPC_EXCL | IPC_CREAT)"
			" failed with %d = %s\n", key, nsems, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%s; semget(0x%x, %d, 0%03o | IPC_EXCL | IPC_CREAT) succeeded"
		" with sem_id = %d\n", __func__, key, nsems, mode, sem_id);
	printf("You can delete the semaphore with: ipcrm -s %d\n", sem_id);

	pid_t pid = 0;
	for (unsigned int seconds = 3; seconds; --seconds)
	{
		pid = fork();
		if (pid == -1)
		{
			printf("Parent failed to create %s. Reason: %s\n",
				"child_process_sem_wait", strerror(errno));
			return EXIT_FAILURE;
		}
		if (pid == 0)		// child
			return child_process(key, seconds);
	}
	// parent

	// wait for all child processes to terminate
	do {
		int status = 0;
		pid = wait(&status);
	} while (pid != -1 && errno != ECHILD);

	// remove the shared memory
	const int result = semctl(sem_id, 0, IPC_RMID);
	if (result == -1)
	{
		fprintf(stderr, "semctl(%d, 0, IPC_RMID) failed with %d = %s\n",
			sem_id, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("semctl(%d, 0, IPC_RMID) succeeded with %d\n", sem_id, result);

	return EXIT_SUCCESS;
}
