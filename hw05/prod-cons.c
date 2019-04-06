#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
  
typedef struct array{
    struct array *node;
      
    int x;
    char *text;
} array_n;
  
  
  
array_n *next;
sem_t sem;
sem_t sem2;
pthread_mutex_t take;
pthread_mutex_t protect; 
int N;
int cons_runs;
int retval;
  
void *cons(void *i){
    sem_wait(&sem2);
    int number = *((int *) i);
    //printf("rec %d\n", number);
    int k=0;
    while (cons_runs){
        while (next != NULL){
             
            pthread_mutex_lock(&take);
            array_n *filter = NULL;
            if (next != NULL){
                filter = next;
                if (next->node != NULL){
                    next = next->node;
                } else {
                    next = NULL;
                }
            }
            pthread_mutex_unlock(&take);
             k++;
            if (filter != NULL){
               // printf("proc %d k %d\n", number, k);
                //printf("waits?\n");
                pthread_mutex_lock(&protect);
                printf("Thread %d:", number);
                int x = filter->x;
                char* text = filter->text;
                for (int j = 1; j < x+1; j++){
                    if (j == x){
                        printf(" %s\n", text);
                    } else{
                        printf(" %s", text);  
                    }
                }
                free(filter->text);
                free(filter);
                pthread_mutex_unlock(&protect);
            }
            
             
              
        }
        sem_post(&sem);
        sem_wait(&sem2);
           
    }
    return NULL;
}
  
  
//producent function
void *prod(){
    int ret, x;
    char *text;
    array_n *filter = NULL;
    //int proc = 0;
    while ((ret = scanf("%d %ms", &x, &text)) == 2) {
        //printf("sem1\n");
        //locking thread and fillin' structure
        if (x < 0){
            //printf("sem\n");
            retval = 1;
            break;
        }
        pthread_mutex_lock(&take);
        array_n *a = malloc(sizeof(array_n));
        a->x = x;
        a->text = text;
        a->node = NULL;
          
        if(next == NULL){
            next = a;
        } else {//fixing iterator FINALLY hope
            filter = next;
            while (filter->node != NULL){
                filter = filter->node;
            }
            filter->node = a;
        }
        cons_runs = 1;
        pthread_mutex_unlock(&take);
        //unlocking consumer
        sem_post(&sem2);
          
    }
    //printf("sem2\n");
     
    while (next != NULL) {
        //printf("sem2\n");
        //lockin producer
        sem_wait(&sem);
    }
    //when prod ends > end consuments
    cons_runs = 0;
      
     
      
    if (ret != EOF){
        retval = 1;
    }
    //waking consuments
    for (int i = 0; i < N; i++){
        sem_post(&sem2);
    }
      
    return NULL;
  
}
  
  
int main(int argc, char *argv[]){
    retval = 0;
    char *conv;
    cons_runs = 0;
    if (argv[1] == NULL){
        N = 1;
    } else{
        N = strtol(argv[1], &conv, 10);
    }
      
    if (N > sysconf(_SC_NPROCESSORS_ONLN)){
        return 1;
    }
      
    //crerating semamphore
    sem_init(&sem, 1, 1);
    sem_init(&sem2, 1, 0);
      
    //creating mutexes
    pthread_mutex_init(&take, NULL);
     
    //second mutex to print
    pthread_mutex_init(&protect, NULL);
      
      
    //creating main thread producent
    pthread_t producent;
    pthread_create(&producent, NULL, prod, NULL);
      
    //creating side threads
    pthread_t consuments[N];
    int j[N];
    for (int i = 0; i < N; i++){
        j[i]= i+1; 
        //printf("j = %d\n", j[i]);
        pthread_create(&consuments[i], NULL, cons, &j[i]);
    }   
      
    //joining consuments threads
    for (int i = 0; i < N; i++){
        pthread_join(consuments[i], NULL);
    }
      
    //joining producent thread
    pthread_join(producent, NULL);
     
 
     
    return retval;
  
      
}
