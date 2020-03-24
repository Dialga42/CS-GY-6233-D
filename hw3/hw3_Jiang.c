#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <wait.h>
int f[2]={0,1};//this array contains the last two numbers of the Fibonacci sequence
const char *NAME="shrmem";
const char *SHRIN="shrin";
const char *SHROUT="shrout";
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
	const int SIZE=n*sizeof(int);
	if (n==0)
	{
		printf("Input Error!\n");
		return 0;
	}
	srand((unsigned)time(NULL));
	int shm_fd=shm_open(NAME,O_CREAT|O_RDWR,0666);
	int shm_in=shm_open(SHRIN,O_CREAT|O_RDWR,0666);
	int shm_out=shm_open(SHROUT,O_CREAT|O_RDWR,0666);
	ftruncate(shm_fd,SIZE);
	ftruncate(shm_in,sizeof(int));
	ftruncate(shm_out,sizeof(int));
	int *ptr=mmap(0,SIZE,PROT_WRITE,MAP_SHARED,shm_fd,0);
	int *in=mmap(0,sizeof(int),PROT_WRITE,MAP_SHARED,shm_in,0);
	int *out=mmap(0,sizeof(int),PROT_READ,MAP_SHARED,shm_out,0);
	pid_t pid=fork();
	if (pid==0)
	{
		*in=0;
		for (int i=1;i<=n;i++)
		{
			while ((*in+1)%n==*out);
			usleep((useconds_t)(rand()%1000000));
			*(ptr+*in)=fib();
			*in=(*in+1)%n;
		}
	}
	else
	{
		shm_fd=shm_open(NAME,O_RDONLY,0666);
		shm_in=shm_open(SHRIN,O_RDONLY,0666);
		shm_out=shm_open(SHROUT,O_RDWR,0666);
		in=mmap(0,SIZE,PROT_READ,MAP_SHARED,shm_in,0);
		out=mmap(0,sizeof(int),PROT_WRITE,MAP_SHARED,shm_out,0);
		ptr=mmap(0,sizeof(int),PROT_READ,MAP_SHARED,shm_fd,0);
		*out=0;
		for (int i=1;i<=n;i++)
		{
			while (*in==*out);
			printf("%d\n",*(ptr+*out));
			*out=(*out+1)%n;
		}
		wait(NULL);
		shm_unlink(NAME);
		shm_unlink(SHRIN);
		shm_unlink(SHROUT);
	}
	return 0;
}