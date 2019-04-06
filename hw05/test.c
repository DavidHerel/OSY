#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include<semaphore.h>

#define DEFAULT_CONSUMERS 1



pthread_mutex_t print_mutex, buffer_mutex;
sem_t wake_consumer, wake_producer;

int number_of_consumers = DEFAULT_CONSUMERS;

typedef struct node {
    char *word;
    unsigned int id;

    struct node *next;
} node_s;

node_s *head;

volatile _Bool consumer_is_runing = true;


void *producer(void *arg) {
    int ret, x;
    char *text;

    while ((ret = scanf("%d %ms", &x, &text)) == 2) {
            pthread_mutex_lock(&buffer_mutex);
            node_s *node = malloc(sizeof(node_s));
            node->word = text;
            node->id = x;
            node->next = NULL;

            if (head == NULL) {
                head = node;
            } else {
                node_s *cursor = head;
                while (cursor->next != NULL) {
                    cursor = cursor->next;
                }
                cursor->next = node;
            }
        pthread_mutex_unlock(&buffer_mutex);

        sem_post(&wake_consumer);
        printf("funguje\n");
    }
    if (ret != 2){
        printf("tu je chuba\n");
    }


    while (head != NULL) {
        sem_wait(&wake_producer);
    }

    consumer_is_runing = false;

    for (int i = 0; i < number_of_consumers; i++) {
        sem_post(&wake_consumer);
    }
    return NULL;
}

void *consumer(void *arg) {
int k = 0;
    int consumer_id = *((int *) arg);
    while (consumer_is_runing) {
    
        while (head != NULL) {
            pthread_mutex_lock(&buffer_mutex);
            node_s *node;
                      k++;
        printf("proc %d k %d\n", consumer_id, k);
        printf("waits?\n");
            if (head != NULL) {
                node = head;
                if (head->next != NULL) {
                    head = head->next;
                } else {
                    head = NULL;
                }
            }
            pthread_mutex_unlock(&buffer_mutex);

            if (node != NULL) {
                pthread_mutex_lock(&print_mutex);
                printf("Thread %d:", consumer_id);
                for (int i = 0; i < node->id; i++) {
                    printf(" %s", node->word);
                }
                printf("\n");
                pthread_mutex_unlock(&print_mutex);


                pthread_mutex_lock(&buffer_mutex);
                if (node->word != NULL) {
                    free(node->word);
                    node->word = NULL;
                }

                free(node);
                node = NULL;
                pthread_mutex_unlock(&buffer_mutex);
            }
        }
        sem_post(&wake_producer);
        sem_wait(&wake_consumer);
    }
    return NULL;
}

int main(int argc, char **argv) {
    char *conv;
    int consumers_no;
    
    if (argv[1] == NULL){
        consumers_no = 1;
    } else{
        consumers_no = strtol(argv[1], &conv, 10);
    }
    
    if (consumers_no > sysconf(_SC_NPROCESSORS_ONLN)){
        return 1;
    }
    
    //creating mutexes
    pthread_mutex_init(&print_mutex, NULL);
    pthread_mutex_init(&buffer_mutex, NULL);

    //crerating shared semaphores
    sem_init(&wake_consumer, 1, 0);
    sem_init(&wake_producer, 1, 1);


    //creating main thread producent
    pthread_t tid_producer;
    pthread_create(&tid_producer, NULL, producer, NULL);


    //creating side threads
    pthread_t tid_consumers[consumers_no];
    int numbers[consumers_no];
    int j[consumers_no];
    for (int i = 0; i < consumers_no; i++){
        j[i] = i + 1;
        pthread_create(&tid_consumers[i], NULL, consumer, &j[i]);
    }  

    //joining producent thread
    pthread_join(tid_producer, NULL);
    
    //joining consuments threads
    for (int i = 0; i < consumers_no; i++) {
        pthread_join(tid_consumers[i], NULL);
    }

}
