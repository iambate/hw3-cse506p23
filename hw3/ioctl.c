#include <asm/unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "ioctl_new.h"
#include <sys/ioctl.h>


int isNumber(char number[])
{
	int i;

	i = 0;
	if (number[0] == '-')
	i = 1;
	for (; number[i] != 0; i++) {
		if (!isdigit(number[i]))
			return -1;
	}
	return 0;
}

int pass_to_kernel(int argc, char **argv)
{
	char *filepath;
	int fdesc = 0;
	int retVal = 0;

	struct var_args *args;
	struct vt_id_list *vt;

	filepath = "/dev/zero";
	fdesc = open(filepath, O_RDONLY);

	if (fdesc < 0) {
		perror("Error in creating a file");
		return -1;
	}

	if (argc == 2) {
		if (strcmp(argv[1], "getall") == 0) {
			vt = (struct vt_id_list *)
					malloc(sizeof(struct vt_id_list));
			vt->vt_ids_info = (char *)malloc(4096);
			vt->vt_ids_info_len = 4096;
			retVal = ioctl(fdesc, GET_FLAG, (unsigned long)vt);
			printf("Number of Vector Tables: %d\n",
							vt->vt_ids_count);
			if (vt->vt_ids_count != 0)
				printf("Details:\n%s\n", vt->vt_ids_info);
			free(vt->vt_ids_info);
			free(vt);
		} else {
			printf("Invalid Arguments\n");
			retVal = -EINVAL;
		}
	}

	else if (argc == 3) {
		if ((strcmp(argv[1], "get") == 0) &&
					(isNumber(argv[2]) == 0)) {
			args = (struct var_args *)
					malloc(sizeof(struct var_args));
			args->process_id = atoi(argv[2]);
			retVal = ioctl(fdesc, GET_VT, (unsigned long)args);
			if (retVal == -888)
				printf("Invalid PID\n");
			else {
				printf("Process ID: %d\n", args->process_id);
				printf("Corresponding Vector Table ID: %d\n",
								args->vector_table_id);
			}
			free(args);
		} else if ((strcmp(argv[1], "get") == 0) &&
					(isNumber(argv[2]) != 0)) {
			printf("Invalid Process ID\n");
			retVal = -EINVAL;
		} else if ((isNumber(argv[1]) == 0) &&
						(isNumber(argv[2]) == 0)) {
			args = (struct var_args *)
					malloc(sizeof(struct var_args));
			args->process_id = atoi(argv[1]);
			args->vector_table_id = atoi(argv[2]);
			retVal = ioctl(fdesc, SET_FLAG, (unsigned long)args);
			if (retVal == -999) {
				printf("Process ID already assigned");
				printf("to the Vector Table ID\n");
			} else if (retVal == -888)
				printf("Invalid PID\n");				
			else if (retVal != 0)
				printf("Invalid Process ID/Vector Table ID: %d\n",
									retVal);
			else {
				printf("Process ID");
				printf(" %d assigned to the Vector Table ID %d\n",
					args->process_id, args->vector_table_id);
			}
			free(args);
		} else {
			printf("Invalid Process ID/Vector Table ID\n");
			retVal = -EINVAL;
		}

	}

	return -retVal;
}


int main(int argc, char **argv)
{
	if ((argc == 2) || (argc == 3))
		pass_to_kernel(argc, argv);
	else
		printf("Invalid number of arguments\n");
	return 0;
}

