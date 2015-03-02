CC = gcc

CFLAGS = -Wall -g

main: main.o IPRIPInterface.o UDPIPInterface.o UDPSocket.o
	$(CC) -Wall -pthread -o main main.o UDPSocket.o ipsum.o IP.o RIP.o IPRIPInterface.o UDPIPInterface.o

UDPSocket.o: UDPSocket.c UDPSocket.h
	$(CC) $(CFLAGS) -c ipsum.c UDPSocket.c

ipsum.o: ipsum.c ipsum.h
	$(CC) $(CFLAGS) -c ipsum.c

IP.o: IP.c IP.h ipsum.h
	$(CC) $(CFLAGS) -c IP.c

RIP.o: RIP.c RIP.h
	$(CC) $(CFLAGS) -c RIP.c

UDPIPInterface.o: UDPIPInterface.c UDPIPInterface.h IP.h UDPSocket.h
	$(CC) $(CFLAGS) -c UDPIPInterface.c

IPRIPInterface.o: IPRIPInterface.c IPRIPInterface.h RIP.h IP.h 
	$(CC) $(CFLAGS) -c RIP.c IP.c IPRIPInterface.c 

clean: 
	$(RM) main *o *~

 
