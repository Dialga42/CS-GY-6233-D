#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
const char *NAME="shrmem";
const int SIZE=5*sizeof(int);
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
	printf("Start address of shared buffer: %llX\n",(unsigned long long)ptr);
	int *in=ptr+3,*out=ptr+4;
	*out=0;
	for (int i=0;i<n;i++)
	{
		while (*in==*out);
		printf("%d\n",ptr[*out]);
		*out=(*out+1)%3;
	}
	shm_unlink(NAME);
    return 0;
}