#include "fib.h"
int f0=0,f1=1;
int fib()
{
	f1=f0^f1;
	f0=f0^f1;
	f1=(f0^f1)+f0;
	return f0;
}