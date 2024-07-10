# C trials

Trials for C programming related questions.

To build the targets start make in this directory.

    .../trials/c $ make


## Prerequisites

- `make` (I recommend gnu make)
- `cc` (I recommend gnu compiler collection)


## File list

|Source         |Target           |Description                                                       |
|---------------|-----------------|------------------------------------------------------------------|
|bypass_fflush.c|bin/bypass_fflush|Are file buffers flushed in case of forgotten fclose, a terminating signal or an exit call?|
|fork.c         |bin/fork         |Shows the use of fork to create several child processes.          |
|forkExec.c     |bin/forkExec     |Shows the use of fork+exec* to start several child programs.      |
|forkExecChild.c|bin/forkExecChild|Sleeps for a while. Default child process binary for bin/forkExec.|
|sem_sysv_readwrite.c|bin/sem_sysv_readwrite|Shows the use of POSIX semaphores. Advanced exclusive and parallel access.|
|sem_sysv_writeonly.c|bin/sem_sysv_writeonly|Shows the use of POSIX semaphores. Simple exclusive access.|
|shm_posix.c    |bin/shm_posix    |Shows the use of POSIX shared memory.                             |
|shm_sysv.c     |bin/shm_sysv     |Shows the use of System V shared memory.                          |
|signal.c       |bin/signal       |Shows the use of signal to handle signals.                        |
|totp.c         |bin/totp         |Implements a Time-based One-Time Password. see RFC 6238.          |
