#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdbool.h>
#define BUFFER_SIZE 5
typedef int buffer_item;
/*the Buffer*/
buffer_item buffer[BUFFER_SIZE];   //create buffer size of 5
sem_t s_empty,s_full;
pthread_mutex_t mutex;
int count,flag, front, end;
void sig_handler(int);

int insert_item(buffer_item item){
  //insert item into the buffer, 0 when successful, -1 when unsuccessful
  if(count < BUFFER_SIZE){ //when the buffer are not full
    buffer[end]=item; //put the item in the buffer
    count++;  //number of item in the buffer increase
    if(end==BUFFER_SIZE-1)  
      {
	end=0;  // for circular array when end=4 start from the begining you would want to edit in the front of the array
      }
    else
      {
	end++; //next item to be add is in the next position
      }
    return 0;
  }
  else{
    printf("FULL BUFFER");
    return -1;
  }
}

int remove_item(buffer_item *item){
  //remove item from the buffer placing it in item, return 0 when successful, -1 when unsuccessful
  if(count>0){   //if there are item in the buffer
    *item=buffer[front];   //assign the element getting remove into item
    count--;
    if(front==BUFFER_SIZE-1)
      {
	front=0;  // for circular array when front ==4 , the next item you would like to remove is the front =0 (the beginning of the array)
      }
    else
      {
	front++; //next item to be remove is the next element in the array
      }
    return 0;
  }else{
    printf("EMPTY BUFFER\n");
    return -1;
  }
}

void *producer(void *param){
  buffer_item item;
  while(1){
    signal(SIGINT,sig_handler);
    int sleeptime=rand()%30;
    sleep(sleeptime);     //sleep from 0 to 30 seconds
    item=rand()%10;       //random a random number 0 to 10 
    sem_wait(&s_empty);
    pthread_mutex_lock(&mutex);   //protect the buffer
    if(insert_item(item)){
      fprintf(stderr,"Error inserting item\n");
    }else{
      printf("Producer produced %d\n",item);     
    }
    pthread_mutex_unlock(&mutex); //release the buffer
    sem_post(&s_full);
    if(flag){
      break;
    }
  }
}

void *consumer(void *param){
  buffer_item item;
  while(1){
    signal(SIGINT,sig_handler);
    int sleeptime=rand()%30;
    sleep(sleeptime);     //sleep from 0 to 20 seconds
    sem_wait(&s_full);     
    pthread_mutex_lock(&mutex);  //protect the buffer
    if(remove_item(&item)){
      fprintf(stderr,"Error:  removing item\n");
    }
    else{
      printf("Consumer consumed %d\n",item);
    }
    pthread_mutex_unlock(&mutex);  //release the buffer
    sem_post(&s_empty);
    if(flag){
      break;
    }
  }
}

void sig_handler(int signo)
{
  if(signo==SIGINT)
    {
      printf("received SIGINT\n");
      flag=1;
    }
}

int main(int argc, char *argv[]){
  if(argc != 4){
    fprintf(stderr,"error:invalid number of arguments\n");
    exit(1);
  }

  if(atoi(argv[1])<0){
    fprintf(stderr,"%d ust be >=0\n",atoi(argv[1]));
    exit(1);
  }
  int sleept=atoi(argv[1]);    //sleeptime
  int producertotal=atoi(argv[2]);   //producer thread total
  int consumertotal=atoi(argv[3]);   //consumer thread total
  //initialation  
  pthread_mutex_init(&mutex,NULL);  //initialize mutex lock

  //create the producer and comsumer threads
  pthread_t ptids[producertotal];
  pthread_t ctids[consumertotal];

  sem_init(&s_empty, 0, BUFFER_SIZE); //initial empty semaphora
  sem_init(&s_full, 0 , 1);           // initial full semaphora
  count=0;
  flag=0;
  front=0;
  end=0;

  printf("Preparing to create %d Producer thread\n",producertotal);
  int i;
  for(i=0;i<producertotal;i++){
    pthread_create(&ptids[i], NULL, producer,NULL);  //create producer thread
    printf("Producer thread #%d created\n",i+1);
  }

  printf("Preparing to create %d Producer thread\n",consumertotal);
  for(i=0;i<consumertotal;i++){
    pthread_create(&ctids[i], NULL, consumer, NULL); //create consumer thread
    printf("Consumer thread #%d created\n",i+1);
  }

  printf("\n");
  //wait for the thread to finish
  for(i=0; i < producertotal; i++){
    pthread_join(ptids[i], NULL);  //wait for producer thread to exit
  }

  for(i=0; i < consumertotal; i++){
    pthread_join(ctids[i], NULL);   //wait for consumer thread to exit
  }

  //sleep before terminate
  sleep(sleept);
  printf("\nFinished!\n");
  return 0; 
}
