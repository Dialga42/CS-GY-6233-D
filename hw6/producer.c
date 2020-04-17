#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "fib.h"
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
    *in=0;
	for (int i=0;i<n;i++)
	{
		while ((*in+1)%3==*out);
		usleep((useconds_t)(rand()%1000000));
		ptr[*in]=fib();
		*in=(*in+1)%3;
	}
    return 0;
}