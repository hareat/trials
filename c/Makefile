.PHONY: all clean

all: bin bin/fork bin/forkExec bin/forkExecChild bin/shm_posix bin/shm_sysv bin/signal

bin:
	mkdir bin

bin/shm_posix: shm_posix.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@ -lrt

bin/%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f bin/*
