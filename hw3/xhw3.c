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
	int rc;
//	struct clone_arg arg;
//       	void *dummy = (void *) argv[1];
	void *stack;
      
	void *stack_start;
	stack = (void*)malloc(65536);
        stack_start=stack+6553;
	unsigned long clone_flags=4096;
	printf("Parent process ID : %d\n", getpid());
	int *i=(int*)malloc(sizeof(int));
	*i=10;
	int *j=(int*)malloc(sizeof(int));
	*j=10;
      	 //rc = syscall(__NR_clone, clone_flags,stack_start,i,j,0)
      	 //;
    rc = syscall(__NR_clone2, SIGCHLD|4096,0,NULL,NULL,0,1);
//	sleep(10000); 
       if (rc == 0)
               printf("syscall returned %d\n", rc);
       else
               printf("syscall returned %d (errno=%d)\n", rc, errno);
free(i);
free(j);
free(stack);

       exit(rc);
}

