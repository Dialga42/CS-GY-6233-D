#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
	srand((unsigned int)time(NULL));
	printf("Hello world!\n%d\n",rand()%100);
	return 0;
}