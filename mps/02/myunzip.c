#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int i = 1;
	char pnum;
	int anum;
	FILE *fp;  

	if (argc < 2) {
   		 printf("Usage: %s FILENAME\n", argv[0]);
   		 exit(1);
 	 }

	while(argv[i] != NULL) {
		fp = fopen(argv[i], "r");

		while(fread(&anum, sizeof(int), 1, fp)) { //gets number of characters
			pnum = fgetc(fp); //gets character to print out  	
			for(int j = 0; j < anum; j++) {
				putchar(pnum); //prints out characters as many times as anum
			}
		}
		fclose(fp);
		++i;
	}
	return 0;
}
