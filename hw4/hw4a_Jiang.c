#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <wait.h>
int f[2]={0,1};//this array contains the last two numbers of the Fibonacci sequence
const int SIZE=5*sizeof(int);
const char *NAME="shrmem";
int fib()
{
	f[1]=f[0]^f[1];
	f[0]=f[0]^f[1];
	f[1]=(f[0]^f[1])+f[0];//swap f[0] and f[1] ,the new f[0] is the one to return
	return f[0];
}
int main(int argc,char* argv[])
{
	if (argc!=2)
	{
		printf("Input Error!\n");
		return 0;
	}
	const int n=atoi(argv[1]);
	if (n==0)
	{
		printf("Input Error!\n");
		return 0;
	}
	srand((unsigned)time(NULL));
	int shm_fd=shm_open(NAME,O_CREAT|O_RDWR,0666);
	ftruncate(shm_fd,SIZE);
	int *ptr=mmap(0,SIZE,PROT_WRITE|PROT_READ,MAP_SHARED,shm_fd,0);
	int *in=ptr+3,*out=ptr+4;
	pid_t pid=fork();
	if (pid==0)
	{
		*in=0;
		for (int i=0;i<n;i++)
		{
			while ((*in+1)%3==*out);
			usleep((useconds_t)(rand()%1000000));
			ptr[*in]=fib();
			*in=(*in+1)%3;
		}
	}
	else
	{
		*out=0;
		for (int i=0;i<n;i++)
		{
			while (*in==*out);
			printf("%d\n",ptr[*out]);
			*out=(*out+1)%3;
		}
		wait(NULL);
		shm_unlink(NAME);
	}
	return 0;
}