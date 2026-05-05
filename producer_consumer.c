#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 5

char arr[SIZE];

int in = 0;
int out = 0;

int total = 0;

int done = 0;

pthread_mutex_t lock;

pthread_cond_t full;
pthread_cond_t empty;

void *producer(void *arg)
{
 FILE *fp;

 char ch;

 fp = fopen("message.txt","r");

 if(fp == NULL){
   printf("file error\n");
   return NULL;
 }

 while((ch = fgetc(fp)) != EOF){

   pthread_mutex_lock(&lock);

   while(total == SIZE)
   {
      pthread_cond_wait(&full,&lock);
   }

   arr[in] = ch;

   in++;

   if(in == SIZE)
      in = 0;

   total++;

   pthread_cond_signal(&empty);

   pthread_mutex_unlock(&lock);

 }

 fclose(fp);

 pthread_mutex_lock(&lock);

 done = 1;

 pthread_cond_signal(&empty);

 pthread_mutex_unlock(&lock);

 return NULL;
}

void *consumer(void *arg)
{
   char ch;

   while(1){

      pthread_mutex_lock(&lock);

      while(total == 0 && done == 0){
         pthread_cond_wait(&empty,&lock);
      }

      if(total == 0 && done == 1){
         pthread_mutex_unlock(&lock);
         break;
      }

      ch = arr[out];

      out++;

      if(out == SIZE)
         out = 0;

      total--;

      pthread_cond_signal(&full);

      pthread_mutex_unlock(&lock);

      printf("%c",ch);

      fflush(stdout);

   }

   return NULL;
}

int main()
{
 pthread_t t1;
 pthread_t t2;

 pthread_mutex_init(&lock,NULL);

 pthread_cond_init(&full,NULL);
 pthread_cond_init(&empty,NULL);

 pthread_create(&t1,NULL,producer,NULL);

 pthread_create(&t2,NULL,consumer,NULL);

 pthread_join(t1,NULL);

 pthread_join(t2,NULL);

 pthread_mutex_destroy(&lock);

 pthread_cond_destroy(&full);
 pthread_cond_destroy(&empty);

 return 0;
}
