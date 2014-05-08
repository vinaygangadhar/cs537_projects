CC   = gcc
OPTS = -Wall

all: server client libmfs.so

# this generates the target executables
server: server.o udp.o helper.o parser.o
	$(CC) -o server server.o udp.o helper.o parser.o 

client: client.o udp.o
	$(CC) -o client client.o udp.o 

libmfs.so: mfs.obj udp.obj
	$(CC) -shared -o libmfs.so mfs.obj udp.obj

server.o: server.c
	$(CC) $(OPTS) -c server.c -o server.o

client.o: client.c
	$(CC) $(OPTS) -c client.c -o client.o 

parser.o: parser.c
	$(CC) $(OPTS) -c parser.c -o parser.o

helper.o: helper.c
	$(CC) $(OPTS) -c helper.c -o helper.o

udp.o: udp.c
	$(CC) $(OPTS) -c udp.c -o udp.o

mfs.obj: mfs.c
	$(CC) $(OPTS) -c -fpic mfs.c -o mfs.obj

udp.obj: udp.c
	$(CC) $(OPTS) -c -fpic udp.c -o udp.obj

clean:
	rm -f server.o udp.o client.o helper.o parser.o server client mfs.obj udp.obj libmfs.so



