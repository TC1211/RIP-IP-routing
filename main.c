#include <stdio.h>
#include <string.h>

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
	int count = 0;
	while (fgets(line, sizeof(line), fr)) {
		printf("\nLINE: %s", line);
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
			count++;
			port = atoi(split);
			printf("PORT: %d\n", port);
			//return client(ipAddr, port);

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
			//create interface, do some other stuff?
		}
		
	}
	fclose(fr);
	return 0;
}