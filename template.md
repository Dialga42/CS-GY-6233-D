Template
====

# Process

## fork

```c
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Fork Failed");
    return 1;
  } else if (pid == 0) {
    execlp("/bin/ls", "ls", NULL);
  } else {
    wait(NULL);
    printf("Child Complete");
  }
  return 0;
}
```

## IPC

### Bounded-Buffer

#### Shared Memory

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/mann.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>

#define IN arr[n];
#define OUT arr[n+1];
#define N 5

int fibnoacci();

int main(int argc, char *argv[]) {
  int pid;
  int ii;
  int n;
  int *arr;
  int fid;
  
  if (argc == 1) {
    printf("Usage: ./lab3 <number of elements>\n");
    return -1;
  }
  n = atoi(argv[1]);
  int SIZE = (N + 2) * sizeof(int);
  fid = shm_open("cs6233", O_CREAT|O_RDWR, 0666);
  ftruncate(fid, SIZE);
  arr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, fid, 0);
  
  IN = 0;
  OUT = 0;
  
  pid = fork();
  
  if (pid == 0) {
    for (ii = 0; ii < n; ii ++) {
      usleep(1000 * (rand() % 1000));
      while(IN + 1) % N == OUT);
      arr[IN] = fibonacci();
      IN = (IN + 1)%N;
    }
  } else if (pid > 0) {
    for (ii = 0; ii < n; ii ++) {
      while (IN == OUT);
      printf("%d ", arr[OUT]);
      fflush(stdout); 
      OUT = (OUT + 1)%N;
    }
    
    wait(NULL);
    shm_unlink("cs6233");
  } else {
    shm_unlink("cs6233");
  }
  
  return 0;
}

int fibonacci() {
  static int ii = 0, outQ = 1, outQQ = 1;
  int out;
  
  if (ii == 0 || ii == 1) {
    ii ++;
    out = 1;
  } else {
  	out = outQ + outQQ;
    outQQ = outQ;
    outQ = out;
  }
  return out;
}
```

#### Sockets

````c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 

#define BUFFER_SIZE 	25
#define READ_END		0
#define WRITE_END		1
#define PORT 8080 

#define SA struct sockaddr 

int ns[2] = {1, 1};
int i = 0;

int fibonacci() {
	int result;
	if (i == 0 || i == 1) {
		result = 1;
	} else {
		result = ns[0] + ns[1];
	}
	ns[i % 2] = result;
	return result;
}

int main(int argc, char const *argv[])
{
	if (argc < 2) {
		printf("usage ./hw5a_song n\n");
		return 1;
	}
	int n = atoi(argv[1]);
	if (n < 0) {
		fprintf(stderr, "ERROR atoi\n");
		return 1;
	}

	struct sockaddr_in server_addr;
	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc == -1) {
		fprintf(stderr, "Could not create socket.\n");
		return 1;
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server_addr.sin_port = htons(PORT);

	if (bind(socket_desc, (SA*) &server_addr, sizeof(server_addr)) != 0) {
		printf("socket bind failed...\n");
		return 1;
	} else {
		printf("socket bind successfully...\n");
	}

	if (listen(socket_desc, 5) != 0) {
		printf("Listen failed ...\n");
		return 1;
	} else {
		printf("Server listening ...\n");
	}

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}

	if (pid == 0) {
		int client_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (client_fd == -1) {
			printf("client socket creation failed...\n");
			return 1;
		} else {
			printf("Client socket successfully created..\n");
		}

		if (connect(client_fd, (SA*) &server_addr, sizeof(server_addr)) != 0) {
			printf("Client's connection with the server failed...\n");
			exit(0);
		} else {
			printf("Client connects to the server...\n");
		}

		int val;
		for (i = 0; i < n; i ++) {
            usleep(1000 * (rand()%1000));
			// write to the shared memory object
			int val = fibonacci();
			write(client_fd, &val, sizeof(int));
		} 
		close(client_fd);
		return 0;
	} else {
		struct sockaddr_in socket_cli;
		int len = sizeof(socket_cli);
		int cond_fd = accept(socket_desc, (SA*)&socket_cli, &len);

		for (i = 0; i < n; i ++) {
			int val = 0;
			read(cond_fd, &val, sizeof(int));
			printf("Server received value from client: %d\n", val);
		}

		wait(NULL);
		close(socket_desc);
		return 0;
	}
}
````

#### pipes

````c
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define READ_END 0
#define WRITE_END 1

int fibonacci();

int main(int args, char *argv[]) {
	int pid;
  int ii;
  int n;
  int fid[2];
  int temp;
  
  if (argc == 1) {
    printf("Usage: ./lab3 <number of elements>\n");		
    return -1;
  }
  n = atoi(argv[1]);
  
  if (pipe(fid)) {
    printf("Error: could not create a communications pipe\n");
    return -1;
  }
  
  pid = fork();
  if (pid == 0) {
    close(fid[READ_END]);
    for (ii = 0; ii < n; ii ++) {
      usleep(1000 * (rand() % 1000));
      temp = fibonacci();
      write(fid[WRITE_END], &temp, sizeof(temp));
    }
    close(fid[WRITE_END]);
  } else if (pid > 0) {
    close(fid[WRITE_END]);
    
    for (ii = 0; ii < n; ii ++) {
      read(fid[READ_END], &temp, sizeof(temp));
    }
    close(fid[READ_END]);
  } else {
    return -1;
  }
  return 0;
}
````

# Thread

## Pthreads Example

```c
#include <pthread.h>
#include <stdio.h>

int sum;
void *runner(void *param);

int main(int args, char *argv[])
{
  pthread_t tid;
  pthread_att_t attr;
  
  if (argc != 2) {
    return -1;
  }
  
  if (atoi(argv[1]) < 0) {
    return -;
  }
  
  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, runner, argv[1]);
  pthread_join(tid, NULL);
}

voido *runner(void *param) {
  int i, upper = atoi(param);
  sum = 0;
  for (i = 1; i <= upper; i ++) {
    sum += i;
  }
  pthread_exit(0);
}
```

# SYNC

## Peterson's solution

- Two process solution

```c
int turn;
bool req[2];
// Pi
do {
	req[0] = true;
	turn = 1;
	while (req[1] && turn == 1);
	// critical section
	req[0] = false;
} while (true);

// Pj
do {
  req[1] = true;
  turn = 0;
  while (req[0] && turn == 0);
  // critical section
  req[1] = false;
} while (true);
```

## test_and_set Instruction

- Shared Boolean variable lock, initialized to FALSE

```c
boolean test_and_set(boolean *target) {
  boolean rv = *target;
  *taregt = TRUE;
  return rv;
}

do {
  while (test_and_set(&lock);
  // ...
  lock = false;
} while (true;
```
### Bounded-waiting Mutual Exclusion

- Previous algorithms didn’t satisfy the bounded wait requirement.

```c
bool waiting[n];
bool lock;

do {
	waiting[i] = true;
	key = true;
	while (waiting[i] && key) {
	  key = test_and_set(&lock);
	}
	waiting[i] = false;
  
	/* critical section */
  
  j = (j + 1) % n;
  while ((j != i) && !waiting[j]) {
    j = (j + 1) % n;
  }
  
	if (j == i) 
    lock = false;
  else
    waiting[j] = false;
} while (true);

```

## compare_and_swap

- Set the variable “value” the value of the passed parameter “new_value” but only if “*value” ==“expected”. That is, the swap takes place only under this condition.

```c
int compare_and_swap(int *value, int expected, int new_value) {
	int temp = *value;
  if (*value == expected)
    *value = new_value;
  return temp;
}

do {
  while (compare_and_swap(&lock, 0, 1) != 0);
  // ...
  lock = 0;
} while (true);
```

## Mutex Locks

- The OS provides abstraction for the hardware tools previously described, particularly since they require some shared lock variables.
- Simplest is mutex.
- Protect a critical section by first **acquire()** a lock then **release()** the lock

- Boolean variable indicating if lock is available or not

- **The entirety of the** **acquire()** **and** **release()** **operations must be atomic**

- Usually implemented via hardware atomic instructions such as test_and_set orcompare_and_swap

## acquire() and release()

### spinlock, busy waiting

```c
acquire() {
	while (!available);
	available = false;
}

release() {
	available = true;
}

int main() {
  do {
    acquire();
    /* critical section */
    release();
  }
}
```

## Semaphore

- Synchronization tool that provides more sophisticated ways (than Mutex locks) for process to synchronize their activities.

- Semaphore **S** – integer variable
- **Theoretically,** it can only be accessed via **indivisible (atomic) operations (shown in** **blue rectangles)** **wait()** and **signal() (**Originally called **P()** and **V())**

### busy waiting

```
wait(S) {
  while (true) {
    if (S > 0) {
      S --;
      break;
    }
  }
}

signal(S) {
  S ++;
}
```

### no busy waiting

```c
wait(semaphore *S) {
	temp1 = S->value --;
	if (temp1 <= 0) {
	  block();
	}
}

signal(semaphore *S) {
	temp2 = S->value ++;
	if (S->value < 0) {
  	/* remove a process P from S->list;*/
	  wakeup(P)
	}
}
```

### Bounded-Buffer problelm

- **n** buffers, each can hold one item
- Semaphore **mutex** initialized to the value 1
- Semaphore **full** initialized to the value 0
- Semaphore **empty** initialized to the value n

```c
// The structure of the producer process
do {
	wait(empty);
	wait(mutex);
	/* add an item to the buffer */ 
  counter++;
  signal(mutex);
  signal(full);/* inc full sem. */ } while (true);
} while(true);

// The structure of the consumer process
do {
	wait(full); /* dec full sem. */
  wait(mutex);
	/* remove an item from buffer */ 
  counter--;
  signal(mutex);
  signal(empty); /*inc empty sem.*/
  /* consume the item in next consumed */
} while (true);
```

### Readers-Writers Problem

- Data set

- Semaphore **rw_mutex** initialized to 1 • Semaphore **mutex** initialized to 1
- Integer **read_count** initialized to 0

```c
// structure of a writer process
do {
	wait(rw_mutex);
  /* writing is performed */
  signal(rw_mutex);
} while (true);

// structure of a reader process
do {
  wait(mutex);
  read_count ++;
  if (read_count == 1)
    wait(rw_mutex);
  signal(mutex);
  
  /* reading dataset is performed, protected by rw_mutex */
  
  wait(mutex);
  read_count --;
  if (read_count == 0)
    signal(rw_mutex);
  signal(mutex);
} while (true);
```

### Dining-Philosophers Problem

```c
do {
  wait (chopstick[i] );
  wait (chopStick[ (i + 1) % 5] );
  // eat
  signal (chopstick[i] );
  signal (chopstick[(i + 1) % 5] );
  //  think
} while (TRUE);

// ************** CAUSE DEADLOCK ****************
```

## Monitors

- The dining philosophers deadlock may be solved using monitors.

- A monitor is a high-level abstraction that provides a convenient and effective mechanism for process synchronization

- *Abstract data type*, internal variables only accessible by code within the procedure

- Only one process may be **active** within the monitor at a time.

```c
 monitor DiningPhilosophers
 {
   	enum {THINKING, HUNGRY, EATING} state[5] ;
   	condition self[5];
		void pickup (int i) { 
      state[i] = HUNGRY;
			test(i); /* if successful: my state becomes EATING + signal myself which is waisted cause I am not waiting*/
			if (state[i] != EATING) self[i].wait(); /* test(i) did not succeed */
    }	
   
		void putdown (int i) { 
      state[i] = THINKING;
			// test left and right neighbors 
      test((i + 4) % 5);
			test((i + 1) % 5);
		}

   	void test (int i) {
			if ((state[(i + 4) % 5] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % 5] != EATING) ) { 
        state[i] = EATING;
				self[i].signal(); 
      }
		}
   
    initialization_code() {
      for (int i = 0; i < 5; i++)
        state[i] = THINKING; 
    }
} /* end of monitor */
```

