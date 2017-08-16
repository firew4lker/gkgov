CFLAGS += -fPIC -Wall `pkg-config gkrellm --cflags`

all: gkgov.so

gkgov.o: gkgov.c
	$(CC) $(CFLAGS) -c gkgov.c

gkgov.so: gkgov.o
	$(CC) -shared -o gkgov.so gkgov.o

install:
	install -m755 gkgov.so ~/.gkrellm2/plugins/

clean:
	rm -rf *.o *.so

# start gkrellm in plugin-test mode
# (of course gkrellm has to be in PATH)
test: gkgov.so
	`which gkrellm` -p gkgov.so

.PHONY: install clean
