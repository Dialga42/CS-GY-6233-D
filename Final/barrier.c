#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <semaphore.h>

// function prototypes
void *thread_func(void *param);
int barrier_point();
int barrier_init();
// number of threads - obtained from an input argument 
int n;
// Variables uses by the barrier
int count;
pthread_mutex_t count_lock;
sem_t semaphore;
int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
		printf("Error - Usage: ./final <number of threads>\n"); 
		return -1;
	}
	int i;
	n = atoi(argv[1]);
    pthread_t thread_ids[n];
    int thread_num[n];
	// Initialize the barrier
	if ( barrier_init() != 0)
	{
		printf("Error: barrier_init() failed\n"); 
		return -1;
	}
	// create the threads
	for (i = 0; i < n; i++) 
	{
		thread_num[i] = i;
		pthread_create(&thread_ids[i], 0, thread_func, &thread_num[i]);
	}
	// wait for the threads to finish 
	for (i = 0; i < n; i++)
    	pthread_join(thread_ids[i], 0);
	return 0; 
}
// The thread function
void *thread_func(void *param)
{
	int seconds;
	int thread_number = *((int*) param);
	/* Sleep for a random period of time */
	seconds = (int) ( (rand() % 5) + 1);
	printf("Thread %d going to sleep for %d seconds\n", thread_number,seconds);
    sleep(seconds);
	/* Wait at the barrier point */
	printf("Thread %d is into the barrier\n", thread_number); 
	barrier_point();
	/* Now we're out of the barrier point */
	printf("Thread %d is out of the barrier\n", thread_number);
	return NULL;
}
int barrier_init()
{
	// Initialize your mutex that protects the counter 
	if (pthread_mutex_init(&count_lock, NULL) != 0) { 
        printf("Error: Could note initialize the pthread_mutex\n"); 
        return -1; 
    }
	// Initialize your semaphore (used for signaling) 
	sem_init(&semaphore, 0, 0);
	// Initialize your counter here
	count=0;
	return 0; 
}
/* The barrier point function */
int barrier_point()
{
	pthread_mutex_lock(&count_lock);
    count++;
    pthread_mutex_unlock(&count_lock); 
    if (count==n)
    	sem_post(&semaphore);
    sem_wait(&semaphore);
    sem_post(&semaphore);
	return 0; 
}