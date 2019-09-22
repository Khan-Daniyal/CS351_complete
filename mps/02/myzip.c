#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv[]) {
	FILE *f;
	int i = 1;
	int printChar = 0;
	char rdr;
	int count = 0;
	

	if (argc < 2) {
    		printf("Usage: %s FILENAME\n", argv[0]);
    		exit(1);
	}

	while(argv[i] != NULL) {
		f = fopen(argv[i], "r+b");
		printChar = rdr = fgetc(f);
		++count;
		
		while(fread(&rdr, sizeof(char), 1, f)) { //while loop reads 1 character at a time of file and puts into rdr
			if(printChar != rdr) { //check if printChar and rdr are same or not
				fwrite(&count, sizeof(int), 1, stdout); //writes out how many times printChar appears
				fputc(printChar, stdout); //writes out actual character	in printChar	
				count = 1; //count set back to 1
				printChar = rdr; //printChar is set to next different character aka rdr value
			}
			else {
				++count; //increment counter bc rdr has same character as printChar
			}
		}
		fclose(f);
		++i;
	}
	fwrite(&count, sizeof(int), 1, stdout);
	fputc(printChar, stdout);
	
	return 0;
}
