#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
int f[2]={0,1};//this array contains the last two numbers of the Fibonacci sequence
const int READ_END=0; 
const int WRITE_END=1;
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
	int fd[2];
	if (pipe(fd)==1) 
	{ 
		printf("Pipe failed\n"); 
		return -1;
	}
	pid_t pid=fork();
	if (pid==0)
	{
		close(fd[READ_END]);
		int now;
		for (int i=0;i<n;i++)
		{
			usleep((useconds_t)(rand()%1000000));
			now=fib();
			write(fd[WRITE_END],&now,sizeof(int));
		}
		close(fd[WRITE_END]);
	}
	else
	{
		close(fd[WRITE_END]);
		int ans;
		for (int i=0;i<n;i++)
		{
			read(fd[READ_END],&ans,sizeof(int)); 
			printf("%d\n",ans);
		}
		wait(NULL);
		close(fd[READ_END]);
	}
	return 0;
}