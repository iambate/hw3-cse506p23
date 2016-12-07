#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


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

#ifndef __NR_clone2
#error clone2 system call not defined
#endif

int main(int argc, const char *argv[])
{
	int rc, fd, i, new_vt_id = 1;
	char *buf = (char *)malloc(4096);
	unsigned long CLONE2_FLAG = 4096;
	printf("Before clone2 call:\nParent process ID : %d\n", getpid());
	printf("Calling read:\n");
	fd = open("test.txt", O_RDONLY);
	read(fd, buf, 4096);
	printf("waiting...\nWhat should be the new VT of cloned process?\n");
	scanf("%d", &new_vt_id);
	//rc = syscall(__NR_clone, clone_flags,stack_start,i,j,0)
	rc = syscall(__NR_clone2, SIGCHLD | CLONE2_FLAG, 0, NULL, NULL, 0, new_vt_id);
	if (rc == 0) {
		printf("syscall returned %d (errno=%d)\n", rc, errno);
		printf("From child: %d\n", getpid());
		printf("Calling read:\n");
		printf("waiting...\nEnter any number to continue...\n");
		scanf("%d", &i);
		read(fd, buf, 4096);
		rc = mkdir("tmp", 0777);
		perror("Error while creating tmp directory");
		printf("waiting...\n");
		scanf("%d", &i);
		printf("From child: %d\n", getpid());
		free(buf);
	} else {
		printf("syscall returned %d\n", rc);
		wait(NULL);
	}
	exit(rc);
}
