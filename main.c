#include <stdio.h>
#include <string.h>

#define INFINITY (16)

struct node_interface {
	int id;
	int port;
	char ipAddr[32];
	char vip[32];
	char status[32]; //up by default
};

struct node_interface interfaces[INFINITY];
int count;

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Incorrect input\n");
		return 1;
	}
	char line[256];
	char *split;
	char *colon;
	char *ipAddr;
	char *vipThis;
	char *vipRemote;
	int port;

	FILE *fr = fopen(argv[1], "r");
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
			port = atoi(split);
			printf("PORT: %d\n", port);
			//return createClient(ipAddr, port);

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

			printf("VIPTHIS: %s\n", vipThis);
			printf("VIPREMOTE: %s\n", vipRemote);
			
			//create interface object
			interfaces[count - 1].id = count;
			interfaces[count - 1].port = port;
			strcpy(interfaces[count - 1].ipAddr, ipAddr);
			strcpy(interfaces[count - 1].vip, vipThis);
			strcpy(interfaces[count - 1].status, "up");

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