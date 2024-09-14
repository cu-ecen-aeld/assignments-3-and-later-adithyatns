#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
int main(int argc, char** argv) {

	int i,len;
	FILE *fp;
	const char *pathname, *mode;
	printf("the count of arguments %d\n", argc);
	//for (i=0;i<argc;i++) printf("the arguments passed %s\n",argv[i]);
	
	pathname = argv[1];

	if(argc < 3)
		printf("./writer <directory> <string to write>\n");
	fp = fopen(argv[1], "w+");
	if(fp == NULL){
		printf("value fo errno apptempting to open file %s: %d\n", pathname, errno); 
		exit(1);
	}

	len = fwrite(argv[2], sizeof(char),strlen(argv[1]),fp) ;
	if (len != strlen(argv[1]))
		printf("write failed\n");

	fclose(fp);

	return 0;
}
