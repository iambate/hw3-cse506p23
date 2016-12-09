#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */
//#include<linux/vector_table.h>
#ifndef __NR_clone2
#error clone2 system call not defined
#endif

int main(int argc, const char *argv[])
{
	int rc, fd, fd1, i, new_vt_id = 1;
	char *buf = (char *)malloc(4096);
	unsigned long CLONE2_FLAG = 4096;
	printf("Before clone2 call:\nParent process ID : %d\n", getpid());
	//intf("The vector_id is :%d",get_vt_id())
	int id,fret;
	printf("Calling read:\n");
	fd = open("test.txt", O_RDONLY);
	fd1 = open("test_1.txt", O_WRONLY);
	read(fd, buf, 4096);
	printf("waiting...\nWhat should be the new VT of cloned process?\n");
	scanf("%d", &new_vt_id);
	//rc = syscall(__NR_clone, SIGCHLD,0,NULL,NULL,0);
	rc = syscall(__NR_clone2, SIGCHLD , 0, NULL, NULL, 0, new_vt_id);
	if (rc == 0) {
		printf("syscall returned %d (errno=%d)\n", rc, errno);
		printf("From child: %d\n", getpid());
		id=syscall(__NR_getvtbyid);
		printf("waiting id in chidl is:%d\n",id);
		fret=syscall(__NR_clone, SIGCHLD|CLONE_SYSCALLS,0,NULL,NULL,0);
		if(fret==0)
		{

			printf("Child's child vtid is:%d\n",syscall(330));
		}
		printf("Calling read:\n");
		printf("waiting...\nEnter any number to continue...\n");
		scanf("%d", &i);
		fd=open("test_read.txt",O_RDONLY);
		read(fd, buf, 4096);
		write(fd1, buf, 4096);
		rc = mkdir("tmp", 0777);
		perror("Error while creating tmp directory");
		printf("waiting...\n");
		scanf("%d", &i);
		printf("From child: %d\n", getpid());
		printf("The vector_id is :%d",id);
		free(buf);
	} else {
		id=syscall(330);
		printf("\nwaiting id in parent is:%d",id);
		printf("syscall returned %d\n", rc);
		wait(NULL);
		free(buf);
	}
	exit(rc);

close(fd1);}
