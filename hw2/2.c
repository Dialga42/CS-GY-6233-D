#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
typedef long long ll;
void fib(ll *a,int n)
{
	for (int i=0;i<n;i++)
	{
		if (i==0||i==1)
			*(a+i)=1;
		else 
			*(a+i)=*(a+i-1)+*(a+i-2);
	}
}
int main(int argc,char* argv[])
{
	if (argc!=2)
	{
		printf("Input Error!\n");
		return 0;
	}
	int n=atoi(argv[1]);
	if (n==0)
	{
		printf("Input Error!\n");
		return 0;
	}
	ll *a=malloc(n*sizeof(ll));
	if (a==NULL)
		printf("Not enough memory!");
	fib(a,n);
	pid_t p=fork();
	if (p==0)
		for (int i=0;i<n;i++)
			printf("%lld%c",*(a+i)," \n"[i==n-1]);
	else
	{
		wait(NULL);
		printf("parent exiting...\n");
	}
	free(a);
	return 0;
}