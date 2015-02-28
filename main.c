#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "UDPSocket.h"

#define MAXENTRY (1024)

struct node_interface {
	int id;
	int port;
	char ipAddr[32];
	char vip[32];
	char status[32]; //up by default
};

struct node_interface interfaces[MAXENTRY];
pid_t children_pid[MAXENTRY];
int count;
int *sock;

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Incorrect input\n");
		return 1;
	}
	parse_file(argv[1]);
	create_listening_sock();
	test_send();
	while(1){};
	kill_all_children();
	close(*sock);
	return 0;
}

int parse_file(char *path){
	struct node_interface *copy = interfaces;
	char line[256];
	char *split, *colon, *ipAddr, *vipThis, *vipRemote;
	int port;
	pid_t pid;

	FILE *fr = fopen(path, "r");
	count = 0;
	while (fgets(line, sizeof(line), fr)) {
		printf("\n%s", line);
		split = strtok(line, ":"); //segment before ":"

		//set up ipAddr variable
		if (strcmp(split, "localhost") == 0) {
			ipAddr = "127.0.0.1";
		} else {
			ipAddr = split;
		}
		printf("IPADDR: %s\n", ipAddr);

		split = strtok(NULL, ":"); //segment after ":"
		
		if (count == 0) { //first line; "port" is this node's port
			port = (uint16_t) atoi(split);
			printf("PORT: %d\n", port);

			int on=1;
			sock = (int *)malloc(sizeof(int));
			create_socket(sock);		
			bind_node_addr(sock, ipAddr, (uint16_t) port);

		} else { //not first line; "port" is remote host's port, and 
			 //there should be VIPs following
			split = strtok(split, " "); 
			port = atoi(split);
			printf("PORT: %d\n", port);
			split = strtok(NULL, " "); //VIP of this node's interface
			if (split == NULL) {
				printf("Incorrect file format\n");
				return 1;
			}
			vipThis = split;

			split = strtok(NULL, " "); //VIP of remote node's interface
			if (split == NULL) {
				printf("Incorrect file format\n");
				return 1;
			}
			vipRemote = split;

			//printf("VIPTHIS: %s\n", vipThis);
			//printf("VIPREMOTE: %s\n", vipRemote);
			
			//create interface object
			copy->id = count;
			copy->port = port;
			strcpy(copy->ipAddr, ipAddr);
			strcpy(copy->vip, vipThis);
			strcpy(copy->status, "up");

			copy++;
			//test:
			ifconfig();
			changeUpDown("down", 1);
			changeUpDown("down", 2); //should fail in 1st run of else branch
		}
		count++;
	}
	fclose(fr);
	return 0;
}

int create_listening_sock(){
	int i = 0;
	while(interfaces[i].id != 0 && i < MAXENTRY){
		//Create a receive process (which for now will only print stuff out)
		pid_t pid = fork();
		if (pid < 0){
			perror("Fork Failed");
			return 1;
		}	
		else if ((int)pid == 0){ //child
			char *received_packet;
			received_packet = (char *)malloc(64000);	
			sock_recv(sock, interfaces[i].ipAddr, (uint16_t) interfaces[i].port, received_packet);
			//for now lets print everything out. will do the ip rip later

		}
		else{
			children_pid[i] = pid;
		}
		i++;
	}
	return 0;
}

int ifconfig() {
	int i = 0;
	for(i = 0; i < count; i++) {
		printf("%d\t%s\t%s\n", interfaces[i].id, interfaces[i].vip, interfaces[i].status);
		//for testing:
		printf(" ipAddr: %s\tport: %d\n", interfaces[i].ipAddr, interfaces[i].port);
	}
	return 0;
}

int changeUpDown (char *upOrDown, int id) {
	int i = 0;
	for(i = 0; i < count; i++) {
		if(id == interfaces[i].id) {
			strcpy(interfaces[i].status, upOrDown);
			printf("Interface %d %s\n", interfaces[i].id, interfaces[i].status);
			//triggered updates here
			return 0;
		}
	}
	printf("Interface %d not found\n", id);
	return 1;
}

int routes() {
	//interact with interface to find and print all next-hops; consult table
}

int send(char *vip, char *message) {
	//find appropriate next hop (consult table) and send
}

int test_send(){
	int i = 0;
	while(i != 10){
	char packet[512] = "baka baka baka!!";
	sock_send(sock, "localhost", 17001, packet);
	i++;
	}
}

int kill_all_children(){
	int i;
	pid_t *copy = children_pid;
	while(*copy > 0){
		*copy = 0;
		kill(*copy,SIGKILL);
		copy++;
	}
	return 0;
}
