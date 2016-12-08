#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i,id;
	printf("From child: %d\n", getpid());
	printf("Enter...\n");
	scanf("%d", &i);
	id = syscall(330);
	printf("Vector Table: %d", id);

return 0;

}
