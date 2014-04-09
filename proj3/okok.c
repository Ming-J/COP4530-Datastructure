/*
Jaime Delgado
COP4610
Project 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
sem_t s_empty, s_full;
pthread_mutex_t mutex;
int count, flag, front, end;

void sig_handler(int sig) {
if(sig == SIGINT) {
printf("\nReceieved SIGINT\n");
/*exit(0)*/
flag = 1;
}
}

int insert_item(int buffer_item) {
if(count < BUFFER_SIZE) {
buffer[end] = buffer_item;
count++;

if(end == BUFFER_SIZE-1)
end = 0;
else
end++;

/*printf("Producer inserts item %d\n", buffer_item);*/
return 0;
} else {
printf("Buffer full, cannot produce\n");
return -1;
}
}

int remove_item(int *buffer_item) {
/*printf("THE COUNT: %d\n", count);*/
if(count > 0) {
*buffer_item = buffer[front];
count--;

if(front == BUFFER_SIZE-1)
front = 0;
else
front++;

/*printf("Consumer removes item %d\n", *buffer_item);*/
return 0;
} else {
printf("Buffer empty, cannot consume\n");
return -1;
}
}

void *producer(void *param) {
int buffer_item;
/* printf("Within Producer\n");*/

while(1) {
signal(SIGINT, sig_handler);

/* sleep for a random period of time */
usleep(rand()%1000000+1);
/* generate random number */
buffer_item = rand()%20+1;

sem_wait(&s_empty);
pthread_mutex_lock(&mutex);

if(insert_item(buffer_item))
fprintf(stderr, "Error inserting item\n");
else
printf("Producer produced %d\n", buffer_item);

pthread_mutex_unlock(&mutex);
sem_post(&s_full);

if(flag)
break;
}
}

void *consumer(void *param) {
int buffer_item;
/* printf("Within Consumer\n");*/
        
while(1) {
signal(SIGINT, sig_handler);

/* sleep for a random period of time */
usleep(rand()%1000000+1);

sem_wait(&s_full);
pthread_mutex_lock(&mutex);

if(remove_item(&buffer_item))
fprintf(stderr, "Error removing item\n");
else
printf("Consumer consumed %d\n", buffer_item);

pthread_mutex_unlock(&mutex);
sem_post(&s_empty);

if(flag)
break;
}
}

int main(int argc, char *argv[]) {

	if(argc != 4) {
fprintf(stderr, "error: invalid number of arguments\n");
exit(1);
}

if(atoi(argv[1]) < 0) {
fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
exit(1);
}

int sleept = atoi(argv[1]);
int ptotal = atoi(argv[2]);
int ctotal = atoi(argv[3]);


  /*
  pthread_mutex_init(&mutex,NULL);

  pthread_t ptids[producertotal];
  pthread_t ctids[consumertotal];

  sem_init(&s_empty, 0, BUFFER_SIZE);
  sem_init(&s_full, 0 , 1);
  count=0;
  flag=0;
  front=0;
  end=0;

  printf("Preparing to create %d Producer thread\n",producertotal);
  int i;
  for(i=0;i<producertotal;i++){
    pthread_create(&ptids[i], NULL, producer,NULL);
    printf("Producer thread #%d created\n",i+1);
  }

  printf("Preparing to create %d Producer thread\n",consumertotal);
  for(i=0;i<consumertotal;i++){
    pthread_create(&ctids[i], NULL, consumer, NULL);
    printf("Consumer thread #%d created\n",i+1);
  }

  printf("\n");

  for(i=0; i < producertotal; i++){
    pthread_join(ptids[i], NULL);
  }

  for(i=0; i < consumertotal; i++){
    pthread_join(ctids[i], NULL);
  }
  */
  sleep(sleept);
  printf("\nFinished!\n");
  return 0; 
}
