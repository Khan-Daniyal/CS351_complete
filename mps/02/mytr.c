#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int replace(char *string1, char *string2);
int delete(char *string1);


int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s STRING1 STRING2\n", argv[0]);
    printf("       %s -d STRING\n", argv[0]);
   
    exit(1);
  }

  char *s1 = argv[1];
  char *s2 = argv[2];
  char *deleteFlag = "-d";

  if(argc > 3) {
	printf("Too many arguments entered\n");
 	exit(1);
   }

 
  if(strcmp(argv[1], deleteFlag) == 0){
	delete(s2);
  }
    
  else if(strlen(s1)!=strlen(s2)){
        printf("STRING1 and STRING2 must have the same length\n");
        exit(1);
  }

  else replace(s1,s2);

  return 0;
}



int replace(char *string1, char *string2){

  int stringLength = strlen(string1);
  char userChar;
  while((userChar = getchar()) != EOF) {
	for(int i=0; i < stringLength; i++){
		if (userChar == string1[i]){
			fputc(string2[i], stdout);
			break;
		}
		else if(i == stringLength - 1){
			fputc(userChar, stdout);
		}
	}
  }

  return 0;

}

int delete(char *string1) {

  int stringLength = strlen(string1);
  char userChar;
  while((userChar = getchar())!= EOF) {
	for(int i = 0; i < stringLength; i++){
		if(userChar == string1[i]){
			break;
		}
		else if (i == stringLength - 1){
			fputc(userChar, stdout);
		}
	}
  }
  return 0;
}






