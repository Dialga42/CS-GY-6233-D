#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void *runner(void *para);
int counter=0,n;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc,char *argv[])
{
	n=atoi(argv[1]);
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,runner,argv[1]);
	for (int i=0;i<n;i++)
	{
		pthread_mutex_lock(&mutex);
		counter++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_join(tid,NULL);
	printf("%10d n=%d\n",counter,n);
	pthread_mutex_destroy(&mutex);
	return 0;
}
void *runner(void *para)
{
	for (int i=0;i<n;i++)
	{
		pthread_mutex_lock(&mutex);
		counter--;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}