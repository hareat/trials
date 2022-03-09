// sem_sysv_readwrite.c shows the use of System V semaphores.
// Create three child processes which want an exclusive access to a shared
// resource (i.e. to write) and three additional child processes which just
// want synchronized access to a shared resource (i.e. to read in parallel
// but not read during a write operation and not write during a read operation)
// and therefore use a semaphore with two semvals.
// In System V a numeric key is the shared information all processes
// or binaries need. If you want to wait and lock ensure that this
// is an atomic operation (semop() with two sembufs).
// Uses fork() to create multiple processes - see fork.c.
// In the trial do not care about removeing system resources
// in the case of an error.
//
// build: cc sem_sysv_readwrite.c -o sem_sysv_readwrite
// use:   ./sem_sysv_readwrite
/////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>	// EXIT_*
#include <string.h>	// strerror
#include <unistd.h>	// fork sleep
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define SEMWRITE  0
#define SEMREAD   1
#define SEMCOUNT  2

#define LOCK    1
#define UNLOCK -1
#define WAIT    0

int child_process_write(const key_t key, const unsigned int seconds)
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
	printf("%*s %s(%u) pid=%d: semget(0x%x, %d, 0%03o) succeeded with sem_id = %d\n",
		indention, "", __func__, seconds, pid, key, nsems, mode, sem_id);

	printf("%*s %s(%u) pid=%d: GETNCNT=%d,%d GETPID=%d,%d GETVAL=%d,%d GETZCNT=%d,%d\n",
		indention, "", __func__, seconds, pid,
		semctl(sem_id, SEMWRITE, GETNCNT), semctl(sem_id, SEMREAD, GETNCNT),
		semctl(sem_id, SEMWRITE, GETPID), semctl(sem_id, SEMREAD, GETPID),
		semctl(sem_id, SEMWRITE, GETVAL), semctl(sem_id, SEMREAD, GETVAL),
		semctl(sem_id, SEMWRITE, GETZCNT), semctl(sem_id, SEMREAD, GETZCNT));

	// wait for read-semval and also wait for write-semval and set a write-lock
	struct sembuf buf3[3] = {SEMREAD, WAIT, SEM_UNDO, SEMWRITE, WAIT, SEM_UNDO, SEMWRITE, LOCK, SEM_UNDO};
	if (semop(sem_id, buf3, 3) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "wait-for-zero and lock", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s %s(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", __func__, seconds, pid, "wait-for-zero and lock");

	for (unsigned int i = 1; i <= seconds; ++i)
	{
		sleep(1);
		printf("%*s %s(%u) pid=%d: slept %d seconds\n", indention, "", __func__, seconds, pid, i);
	}

	struct sembuf buf = {SEMWRITE, UNLOCK, SEM_UNDO};
	if (semop(sem_id, &buf, 1) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "unlock", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s %s(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", __func__, seconds, pid, "unlock");

	printf("%*s %s(%u) pid=%d: GETNCNT=%d,%d GETPID=%d,%d GETVAL=%d,%d GETZCNT=%d,%d\n",
		indention, "", __func__, seconds, pid,
		semctl(sem_id, SEMWRITE, GETNCNT), semctl(sem_id, SEMREAD, GETNCNT),
		semctl(sem_id, SEMWRITE, GETPID), semctl(sem_id, SEMREAD, GETPID),
		semctl(sem_id, SEMWRITE, GETVAL), semctl(sem_id, SEMREAD, GETVAL),
		semctl(sem_id, SEMWRITE, GETZCNT), semctl(sem_id, SEMREAD, GETZCNT));

	return EXIT_SUCCESS;
}


int child_process_readonly(const key_t key, const unsigned int seconds)
{
	const int pid = getpid();
	const unsigned int indention = 0;	// to emphasize the parallelism
	const int nsems = 0;	// don't care because we depend on an existing one
	const int mode = 0; 	// don't care because we depend on an existing one
	const int sem_id = semget(key, nsems, mode);
	if (sem_id == -1)
	{
		fprintf(stderr, "%s(%u): semget(0x%x, %d, 0%03o) failed with %d = %s\n",
			__func__, seconds, key, nsems, mode, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s %s(%u) pid=%d: semget(0x%x, %d, 0%03o) succeeded with sem_id = %d\n",
		indention, "", __func__, seconds, pid, key, nsems, mode, sem_id);

	printf("%*s %s(%u) pid=%d: GETNCNT=%d,%d GETPID=%d,%d GETVAL=%d,%d GETZCNT=%d,%d\n",
		indention, "", __func__, seconds, pid,
		semctl(sem_id, SEMWRITE, GETNCNT), semctl(sem_id, SEMREAD, GETNCNT),
		semctl(sem_id, SEMWRITE, GETPID), semctl(sem_id, SEMREAD, GETPID),
		semctl(sem_id, SEMWRITE, GETVAL), semctl(sem_id, SEMREAD, GETVAL),
		semctl(sem_id, SEMWRITE, GETZCNT), semctl(sem_id, SEMREAD, GETZCNT));

	// wait for write-semval but do not wait for read-semval
	struct sembuf buf2[] = {SEMWRITE, WAIT, SEM_UNDO, SEMREAD, LOCK, SEM_UNDO};
	if (semop(sem_id, buf2, 2) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "write-wait-for-zero and started reading", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s %s(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", __func__, seconds, pid, "write-wait-for-zero and started reading");

	for (unsigned int i = 1; i <= seconds; ++i)
	{
		sleep(1);
		printf("%*s %s(%u) pid=%d: slept %d seconds\n", indention, "", __func__, seconds, pid, i);
	}

	struct sembuf buf = {SEMREAD, UNLOCK, SEM_UNDO};
	if (semop(sem_id, &buf, 1) == -1)
	{
		fprintf(stderr, "%s(%u): semop(%s) failed with %d = %s\n",
			__func__, seconds, "finished reading", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%*s %s(%u) pid=%d: semop(%s) succeeded\n",
		indention, "", __func__, seconds, pid, "finished reading");

	printf("%*s %s(%u) pid=%d: GETNCNT=%d,%d GETPID=%d,%d GETVAL=%d,%d GETZCNT=%d,%d\n",
		indention, "", __func__, seconds, pid,
		semctl(sem_id, SEMWRITE, GETNCNT), semctl(sem_id, SEMREAD, GETNCNT),
		semctl(sem_id, SEMWRITE, GETPID), semctl(sem_id, SEMREAD, GETPID),
		semctl(sem_id, SEMWRITE, GETVAL), semctl(sem_id, SEMREAD, GETVAL),
		semctl(sem_id, SEMWRITE, GETZCNT), semctl(sem_id, SEMREAD, GETZCNT));

	return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
	const key_t key = 0x1234;	// hex is common
	const int nsems = SEMCOUNT;
	const int mode = 0644;   	// octal is common

	// IPC_EXCL | IPC_CREAT should ensure sem is created by this process.
	const int sem_id = semget(key, nsems, mode | IPC_EXCL | IPC_CREAT);
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
	for (unsigned int seconds = 6; seconds; --seconds)
	{
		pid = fork();
		if (pid == -1)
		{
			printf("Parent failed to create %s. Reason: %s\n",
				"child_process_sem_wait", strerror(errno));
			return EXIT_FAILURE;
		}
		if (pid == 0)		// child
			if (seconds % 2)
				return child_process_readonly(key, seconds);
			else
				return child_process_write(key, seconds);
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
