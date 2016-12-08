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
#include <sys/ioctl.h>

int pass_to_kernel(int argc, char **argv){

	char* filepath;
	int fdesc = 0;
	int retVal = 0;
	int i;
		
	struct var_args *args;
	struct vt_id_list *vt;
	
	//args->process_id = NULL;
	//args->vector_table_id = NULL;

	filepath = "/dev/zero";

	if ((fdesc = open(filepath, O_RDONLY)) < 0) {
			perror("Error in creating a file");
			return -1;
	}

	if (argc == 2) {
		if(strcmp(argv[1], "getall") == 0) {
			vt = (struct vt_id_list*)malloc(sizeof(struct vt_id_list));
			vt->vt_ids_info = (char *)malloc(4096);
			vt->vt_ids_info_len = 4096;
			printf("Argument = getall\n");
			retVal = ioctl(fdesc,GET_FLAG,(unsigned long)vt);
			perror("return");
			printf("Return Value of ioctl: %d\n",retVal);
			printf("Number of Vector Tables: %d\n", vt->vt_ids_count);
			printf("Details: %s\n", vt->vt_ids_info);
			free(vt->vt_ids_info);
			free(vt);
		}
	}
	
	else if(argc == 3) {
		args = (struct var_args*)malloc(sizeof(struct var_args));
		args->process_id = atoi(argv[1]);
		args->vector_table_id = atoi(argv[2]);
		printf("Process ID: %d\n", args->process_id);
		printf("Vector Table: %d\n", args->vector_table_id);
		retVal = ioctl(fdesc,SET_FLAG,(unsigned long)args);
		//printf("arg address=%lu\n",(unsigned long)args);
		perror("return");
		printf("Invalid Vector Table: Error: %d\n",retVal);
	}

	return 0;
}


int main(int argc, char **argv)
{
	printf ("Number of Arguments = %d\n", argc);
	
	if ((argc == 2) || (argc == 3)){
		pass_to_kernel(argc, argv);		
	}		
	else 
		printf("Invalid number of arguments\n");	
		
	return 0;
}

//testing 



