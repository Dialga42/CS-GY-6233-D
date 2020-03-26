#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <wait.h>
#include <string.h>
const int READ_END=0; 
const int WRITE_END=1;
int main()
{
	int p1[2],p2[2];
	if (pipe(p1)==1||pipe(p2)==1)
	{
		printf("Pipe failed\n"); 
		return -1;
	}
	pid_t pid=fork();
	if (pid<0)
	{
		printf("Fork failed\n"); 
		return -1;
	}
	if (pid==0)
	{
		close(p1[READ_END]);
		char c='a';
		write(p1[WRITE_END],&c,sizeof(char));
		close(p1[WRITE_END]);
		close(p2[WRITE_END]);
		char out[200];
		int pos=0;
		while (read(p2[READ_END],&c,sizeof(char))&&c!='\n')
			out[pos++]=c;
		out[pos++]=c;
		out[pos]='\0';
		printf("%s",out);
		close(p2[READ_END]);
	}
	else
	{
		close(p1[WRITE_END]);
		char c;
		read(p1[READ_END],&c,sizeof(char));
		char *s="Hello world, this is CS6233\n"; 
		close(p2[READ_END]);
		write(p2[WRITE_END],s,strlen(s));
		wait(NULL);
		close(p1[READ_END]);
		close(p2[WRITE_END]);
	}
	return 0;
}