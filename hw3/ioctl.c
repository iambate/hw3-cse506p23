#include <asm/unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "ioctl_new.h"

int pass_to_kernel(int argc, char **argv){

	char* filepath;
	int fdesc = 0;
	int retVal = 0;
		
	struct var_args *args;
	args = (struct var_args*)malloc(sizeof(struct var_args));
	
	args->process_id = NULL;
	args->vector_table = NULL;
	args->all = 0;	

	if (argc == 2){
		if(strcmp(argv[1], "getall") == 0){
			printf("Argument = getall\n");
			args->all = 1;		
			printf("args->all: %d\n", args->all);
		}
	}	
	
	else if(argc == 3) {
		args->process_id = argv[1];
		args->vector_table = argv[2];
		printf("Process ID: %s\n", args->process_id);
		printf("Vector Table: %s\n", args->vector_table);
	}
	
	filepath = "/dev/zero"; 

	if ((fdesc = open(filepath, O_RDONLY)) < 0) {
			perror("Error in creating a file");
			return -1;
	}

	retVal = ioctl(fdesc,SET_FLAG,(unsigned long)&args);
	printf("arg address=%lu\n",(unsigned long)&args);
	perror("return");
	printf("Return Value of ioctl: %d",retVal);

	//retVal = ioctl(fdesc,GET_FLAG,&args);

	return 0;
}


int main(int argc, char **argv)
{
	char *process_id;
	char *vector_table;
	printf ("Number of Arguments = %d\n", argc);
	
	if ((argc == 2) || (argc == 3)){
		pass_to_kernel(argc, argv);		
	}		
	else 
		printf("Invalid number of arguments\n");	
		
	return 0;
}





