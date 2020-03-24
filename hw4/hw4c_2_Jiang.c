#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void *runner(void *para);
int counter=0,n=0;
int main(int argc,char *argv[])
{
	while (1)
	{	
		++n;
		counter=0;
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tid,&attr,runner,argv[1]);
		for (int i=0;i<n;i++)
			counter++;
		pthread_join(tid,NULL);
		if (counter)
		{
			printf("%10d n=%d\n",counter,n);
			break;
		}
	}
	return 0;
}
void *runner(void *para)
{
	for (int i=0;i<n;i++)
		counter--;
	pthread_exit(0);
}