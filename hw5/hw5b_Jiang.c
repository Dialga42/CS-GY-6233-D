#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void *runner(void *para);
int counter=0,n;
_Bool turn,req[2];
int main(int argc,char *argv[])
{
	n=atoi(argv[1]);
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,runner,argv[1]);
	for (int i=0;i<n;i++)
	{
		req[0]=1;
		turn=1;
		while (req[1]&&turn==1);
		counter++;
		req[0]=0;
	}
	pthread_join(tid,NULL);
	printf("%10d n=%d\n",counter,n);
	return 0;
}
void *runner(void *para)
{
	for (int i=0;i<n;i++)
	{
		req[1]=1;
		turn=0;
		while (req[0]&&turn==0);
		counter--;
		req[1]=0;
	}
	pthread_exit(0);
}