
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS	4
#define MAX 1024

void *sub_string(void *);
int readf(FILE *fp);
int total=0;
int nlocal,n1,n2;
char *s1,*s2;
FILE *fp;
pthread_mutex_t total_lock;

int main(int argc, char *argv[])
{
	int i,rc;
	pthread_t threads[NUM_THREADS];

	pthread_mutex_init(&total_lock,NULL);
	readf(fp);
	for(i=0;i<NUM_THREADS;i++){
		rc=pthread_create(&threads[i],NULL,sub_string,(void *)(long)i);
		if (rc){
			printf("ERROR: return error from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(i=0; i<NUM_THREADS; i++){
		rc = pthread_join(threads[i], NULL);
		if (rc){
			printf("ERROR: return error from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}
	printf("the occurences of s2 in s1 is %d\n",total);
	pthread_exit(0);
}



int readf(FILE *fp)
{
	if((fp=fopen("strings.txt", "r"))==NULL){
		printf("ERROR: can't open string.txt!\n");
		return 0;
	}
	s1=(char *)malloc(sizeof(char)*MAX);
	if(s1==NULL){
		printf("ERROR: Out of memory!\n");
		return -1;
	}
	s2=(char *)malloc(sizeof(char)*MAX);
	if(s2==NULL){
		printf("ERROR: Out of memory\n");
		return -1;
	}
	/*read s1 s2 from the file*/
	s1=fgets(s1, MAX, fp);
	s2=fgets(s2, MAX, fp);
	n1=strlen(s1);  /*length of s1*/
	n2=strlen(s2)-1; /*length of s2*/
	nlocal=n1/NUM_THREADS;  /*data length held by process*/
	if(s1==NULL || s2==NULL ||n1<n2)  /*when error exit*/
		return -1;
}

void *sub_string(void *threadid){

    long id = (long)threadid;

    int left = id * nlocal;
    int right;

    int i, j;
    int found;

    int matches = 0;

    if(id == NUM_THREADS - 1)
        right = n1 - n2;
    else{
        right = left + nlocal - 1;
    }

    for(i = left; i <= right; i++){

        found = 1;

        for(j = 0; j < n2; j++){

            if(s1[i + j] != s2[j]){
                found = 0;
                break;
            }

        }

        if(found == 1){
            matches++;
        }

    }

    pthread_mutex_lock(&total_lock);

    total += matches;

    pthread_mutex_unlock(&total_lock);

    return NULL;
}
