.PHONY: all clean

all: bin tmp bin/bypass_fflush bin/fork bin/forkExec bin/forkExecChild \
	bin/sem_sysv_readwrite bin/sem_sysv_writeonly bin/shm_posix bin/shm_sysv bin/signal

bin:
	mkdir bin

tmp:
	mkdir tmp

bin/shm_posix: shm_posix.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@ -lrt

bin/%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f bin/*
