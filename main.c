#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Incorrect input\n");
		return 1;
	}
	char line[256];
	char *colon;
	char *space;
	int count;

	FILE *fr = fopen(argv[1], "r");
	count = 0;
	while (fgets(line, sizeof(line), fr)) {
		printf("%s", line);
		colon = strtok(line, ":");
		colon = strtok(NULL, ":");
		if (count == 0) {
			printf("\t%s", colon);
			count++;
		} else {
			space = strtok(colon, " ");
			while(space != NULL) {
				if (strstr(space, ".")) 
					printf("\t\t%s\n", space);
				else
					printf("\t%s\n", space);
				space = strtok(NULL, " ");
			}
		}
	}
	fclose(fr);
	return 0;
}