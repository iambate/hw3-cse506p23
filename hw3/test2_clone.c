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
#include "header.h"
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
	int rc, fd, fd1, i, new_vt_id = 1;
	char *buf = (char *)malloc(4096);
	int id, fret;

	printf("Before clone2 call:\n");

	fd = open("test.txt", O_RDONLY);
	fd1 = open("a.protected", O_CREAT);

	/*Calling parent's read*/
	read(fd, buf, 4096);
	printf("waiting...\n What should be the new VT of cloned process?\n");
	scanf("%d", &new_vt_id);
	rc = syscall(__NR_clone2, SIGCHLD, 0, NULL, NULL, 0, new_vt_id);
	if (rc == 0) {
		printf("Calling child of child");
		printf("(To test the non default behavior(CLONE_SYSCALLS flag) of inheritance)\n");
		fret = syscall(__NR_clone, SIGCHLD|CLONE_SYSCALLS, 0, NULL, NULL, 0);
		if (fret == 0) {

			printf(" Child's child (grand child) has pid: %d, ppid:% d\n", getpid(), getppid());
			printf(" Child's child (grand child) vector table id is: %lu\n", syscall(__NR_getvtbyid));
			free(buf);
			close(fd);
			close(fd1);
			return 0;
		}
		wait(NULL);
		printf("In child has pid :%d, ppid: %d\n", getpid(), getppid());
		id = syscall(__NR_getvtbyid);
		printf("Vector id of child is: %d\n", id);
		printf("Calling link/unlink of protected file\n");
		link("a.protected", "b.txt");
		unlink("a.protected");
		printf("Calling read for child 1:\n");
		printf("waiting...\nEnter any number to continue...\n");
		scanf("%d", &i);
		fd = open("test_read.txt", O_RDONLY);
		read(fd, buf, 4096);
		rc = mkdir("tmp", 0777);
		perror("Error while creating tmp directory");
		printf("waiting...\n");
		scanf("%d", &i);
	} else {
		wait(NULL);
		id = syscall(__NR_getvtbyid);
		printf("\nVector id in main process is: %d\n", id);
		printf("main process pid is: %d and ppid is :%d\n", getpid(), getppid());
		printf("syscall returned %d\n", rc);
	}
	free(buf);
	close(fd);
	close(fd1);
	exit(rc);
}
