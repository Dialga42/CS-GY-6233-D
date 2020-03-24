#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
int f[2]={0,1};//this array contains the last two numbers of the Fibonacci sequence
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
		return -1;
	}
	int cfd,fd=socket(AF_UNIX,SOCK_STREAM,0);
	if (fd<0)
	{
		printf("Socket Error!\n");
		return -1;
	}
	struct sockaddr_un serv,client;
	socklen_t len=sizeof(client);
	serv.sun_family=AF_UNIX;
	strcpy(serv.sun_path,"Server Socket");
	pid_t pid=fork();
	if (pid<0)
	{
		printf("Fork Error!\n");
		return -1;
	}
	if (pid==0)
	{
		cfd=socket(AF_UNIX,SOCK_STREAM,0);
		if (cfd<0)
		{
			printf("Socket Error!\n");
			return -1;
		}
		client.sun_family=AF_UNIX;
		strcpy(client.sun_path,"Client Socket");
		if (bind(cfd,(struct sockaddr*)&client,sizeof(client))<0)
		{
			printf("Bind Error!\n");
			return -1;
		}
		if (connect(cfd,(struct sockaddr*)&serv,sizeof(serv))<0)
		{
			printf("Connect Error!\n");
			return -1;
		}
		int now;
		for (int i=0;i<n;i++)
		{
			now=fib();
			write(cfd,&now,sizeof(int));
		}
		unlink("Client Socket");
	}
	else
	{
		if (bind(fd,(struct sockaddr*)&serv,sizeof(serv))<0)
		{
			printf("Bind Error!\n");
			return -1;
		}
		if (listen(fd,6)<0)
		{
			printf("Listen Error!\n");
			return -1;
		}
		cfd=accept(fd,(struct sockaddr*)&client,&len);
		if (cfd<0)
		{
			printf("Accept Error!\n");
			return -1;
		}
		int ans;
		for (int i=0;i<n;i++)
		{
			read(cfd,&ans,sizeof(int)); 
			printf("%d\n",ans);
		}
		wait(NULL);
	}
	unlink("Server Socket");
	close(fd);
	close(cfd);
	return 0;
}