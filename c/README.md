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
|fork.c         |bin/fork         |Shows the use of fork to create several child processes.          |
|forkExec.c     |bin/forkExec     |Shows the use of fork+exec* to start several child programs.      |
|forkExecChild.c|bin/forkExecChild|Sleeps for a while. Default child process binary for bin/forkExec.|
|signal.c       |bin/signal       |Shows the use of signal to handle signals.                        |
