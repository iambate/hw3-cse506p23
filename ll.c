#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include "sys_xmergesort.h"

#ifndef __NR_xmergesort
#error xmergesort system call not defined
#endif
int main(int argc, char **argv)
{
        struct argument arg;
	//int flagentered;
	int rc;
        char c;
        int flag=0;
	int index=0;
        int count=0;
        int k=0;
FILE *fp,*fp1;
fp=fopen("test.txt", "r");

printf("%d",fp);
fp1=fopen("test.txt", "r");
printf("%d",fp1);
        while ((c = getopt (argc, argv, "uaitd")) != -1)
    		switch (c)
      		{
      			case 'u':
        			 flag=flag|0x01 ;
       				 break;
      			case 'a':
        			flag=flag|0x02 ;
                         	break;
        		case 'i':
        			flag=flag|0x04;
        			break;
                        case 't':
                        	flag=flag|0x10;
                        	break;
                        case 'd':
                        	flag=flag|0x20;
                        	break;
                        default:
                        	abort ();
     		 }
        
         arg.outfile=argv[optind];  	  
     	 for (index = optind+1; index < argc; index++)
	{
       		arg.inputfile[k++]=argv[index];
	        count=count+1;	
	}

        arg.data=(int*)malloc(sizeof(int));
        arg.noofinfile=count;
        arg.flag= flag;
	if(flag==0)
	printf("\nWrong arguments entered!Enter atleast 1 flag");
	
  	rc = syscall(__NR_xmergesort,(void*)&arg);
	if (rc == 0){

                printf("syscall returned %d\n", rc);
		if(flag&0x20)
		printf("\nData at user space is:%d",*(arg.data));
		}
	else
	{
		printf("syscall returned %d (errno=%d)\n", rc, errno);
                perror("Message from Syscall");
	}


//	free(arg->data);
	//free(arg);
	exit(rc);
}
