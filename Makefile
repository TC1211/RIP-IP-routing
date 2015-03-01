CC = gcc

CFLAGS = -Wall -g

default: main

main: IP.o IPRIPInterface.o ipsum.o RIP.o UDPIPInterface.o UDPSocket.o
	$(CC) $(CFLAGS) -o main IP.o IPRIPInterface.o ipsum.o RIP.o UDPIPInterface.o UDPSocket.o

UDPSocket.o: UDPSocket.c UDPSocket.h
	$(CC) $(CFLAGS) -c UDPSocket.c

ipsum.o: ipsum.c ipsum.h
	$(CC) $(CFLAGS) -c ipsum.c

IP.o: IP.c IP.h ipsum.h
	$(CC) $(CFLAGS) -c IP.c

UDPIPInterface.o: UDPIPInterface.c UDPIPInterface.h IP.h UDPSocket.h
	$(CC) $(CFLAGS) -c UDPIPInterface.c

RIP.o: RIP.c RIP.h
	$(CC) $(CFLAGS) -c RIP.c

IPRIPInterface.o: IPRIPInterface.c RIP.h IP.h
	$(CC) $(CFLAGS) -c IPRIPInterface.c

clean: 
	$(RM) main *o *~

 
