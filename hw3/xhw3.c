#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef __NR_clone2
#error clone2 system call not defined
#endif

int main(int argc, const char *argv[])
{
	int rc;
//	struct clone_arg arg;
//       	void *dummy = (void *) argv[1];
	void *stack = NULL;
       	void *stack_start;
	stack = (void*)malloc(65536);
        stack_start=stack+6553;
	unsigned long clone_flags=4096;
	printf("Parent process ID : %d\n", getpid());
	
       rc = syscall(__NR_clone2, clone_flags,stack_start,NULL,NULL,0,0);
       if (rc == 0)
               printf("syscall returned %d\n", rc);
       else
               printf("syscall returned %d (errno=%d)\n", rc, errno);

       exit(rc);
}

