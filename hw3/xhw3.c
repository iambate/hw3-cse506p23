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
	long a, b, c, d;
	char *buf = (char *)malloc(4096);
	int id,fret;
	printf("Calling read:\n");

	printf("waiting...\nWhat should be the new VT of cloned process?\n");
	scanf("%d", &new_vt_id);

	rc = syscall(__NR_clone2, SIGCHLD , 0, NULL, NULL, 0, new_vt_id);
	if (rc == 0) {
		printf("syscall returned %d (errno=%d)\n", rc, errno);

		id = syscall(__NR_getvtbyid);
		printf("waiting id in chidl is:%d\n",id);

		fret = syscall(__NR_clone, SIGCHLD,0,NULL,NULL,0);

		if(fret==0)
		{

			printf("Child's child vtid is:%ld\n",syscall(330));
			printf("waiting...\nEnter any number to continue...\n");
			scanf("%d", &i);
			return 0;
		}
		else {
			printf("PID 1: %d\n", getpid());
			wait(NULL);
		}

		wait(NULL);

		printf("waiting...\nEnter any number to continue...\n");
		scanf("%d", &i);

		fd = open("test.txt", O_RDONLY);
		fd1 = open("test_1.txt", O_WRONLY);
		printf("fd = %u, fd1 = %u\n",fd, fd1);
		a = read(fd, buf, 5);

		printf("read retrun %ld\n", a);
		
 		b = write(fd1, buf, 5);
		printf("write return %ld\n", b);
		
		c = mkdir("tmp", 0777);
		printf("mkdir return %ld\n", c);
		
		d = rmdir("tmp");
		printf("rmdir return %ld\n", d);
		close(fd);
		close(fd1);
		printf("waiting...\n");
		scanf("%d", &i);
		printf("From child: %d\n", getpid());
		printf("The vector_id is :%d\n",id);
		free(buf);
	} else {
		wait(NULL);
		printf("PID 2: %d\n", getpid());
		id=syscall(330);
		printf("\nwaiting id in parent is:%d\n",id);
		printf("syscall returned %d\n", rc);
		free(buf);
	}
	return 0;
}
