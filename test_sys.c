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

#define STACK_SIZE (65536)

clone_body(void *arg) {
	int ret1 = 0;
	printf("Child process ID : %d\n", getpid());
//	ret1 = open("test_clone", 66, 77);
	//printf("child return value is: %d\n\n", ret1);
	exit(0);
}
int main(int argc, char **argv)
{
      
	int rc;
        char c;
        int flag=0;
	int index=0;
        int count=0;
        int k=0;
	char *buff;
	void *stack = NULL;
	void *stack_start;
	buff = malloc(52*sizeof(char));
	int ret;
	int i=2;
	printf("\nTesting clone() for \"open\" only. So need to use \n");
	printf("Parent process ID : %d\n", getpid());
//	ret = open("trctl", 22, 33);
//	printf("parent return value for open call is: %d\n\n", ret);
	stack = (void*)malloc(65536);
	stack_start=stack+6553;
       
       //stackTop = stack + STACK_SIZE;
	int *j;

	// sleep(1);
	 
	printf("Calling clone .. \n");
			
//	memcpy(stack_start, &i, sizeof(int));
//	j=stack_start;
//	printf("stack start pointer:%lu\n",j);
//	printf("Stack_start:%lu\n,StackTop %lu\n",*j,stack_start);
	//rc=clone(CLONE_FILES,stack_start,NULL,NULL,0); 
	rc = clone(&clone_body, stack_start,CLONE_FS|4096, NULL);
	printf("%d",rc);
	// 				if(ret < 0) {
	// 						printf("ERROR:  Ret of clone is negative\n\n");
	// 							}
	//
/*	rc = open("/mnt/trfs/hw1/test_hw2.txt", O_CREAT | O_RDWR | O_APPEND, 0644);
  
	k = read(rc, buff, 12);
  k = write(rc, buff, 12);

  rc = mkdir("/mnt/trfs/hw1/test_hw2_dir_1/", 0644);
  printf("rc=%d\n",rc);
  rc = rmdir("/mnt/trfs/hw1/test_hw2_dir_1/");
  printf("rc=%d\n",rc);

  rc = rename("/mnt/trfs/hw1/test_hw2.txt", "/mnt/trfs/hw1/test_hw2_renamed.txt");
  printf("rc=%d\n",rc);
  rc = rename("/mnt/trfs/hw1/test_hw2_renamed.txt", "/mnt/trfs/hw1/test_hw2.txt");
  printf("rc=%d\n",rc);  

  rc = link("/mnt/trfs/hw1/test_hw2.txt", "/mnt/trfs/hw1/test_hw2_hardlink.txt");
  printf("rc=%d\n",rc);

  rc = unlink("/mnt/trfs/hw1/test_hw2_hardlink.txt");
  printf("rc=%d\n",rc);

  rc = symlink("/mnt/trfs/hw1/test_hw2.txt", "/mnt/trfs/hw1/test_hw2_symlink11.txt");
  printf("rc=%d\n",rc);

  rc = readlink("/mnt/trfs/hw1/test_hw2_symlink11.txt", buff, 52);
  printf("rc=%d\n",rc);

  rc = unlink("/mnt/trfs/hw1/test_hw2_symlink11.txt");
  printf("rc=%d\n",rc);
	
	free(buff);
  close(rc);

*/	exit(rc);
}
