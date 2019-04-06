#include "worker_provider.h"

#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "worker.h"
#include "workplace_provider.h"

typedef struct running_worker {
    pthread_t thread;
    worker_info_t *worker_info;
    struct running_worker *next;
} running_worker_t;

running_worker_t *running_workers_head = NULL;

_Bool is_somebody_working() {
    running_worker_t *cursor = running_workers_head;

    while (cursor != NULL) {
        if (cursor->worker_info->is_working) {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

_Bool contains_worker(workplace_type type) {
    running_worker_t *cursor = running_workers_head;

    while (cursor != NULL) {
        if (cursor->worker_info->type == type) {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

void wake_up_workers(workplace_type type) {
    running_worker_t *cursor = running_workers_head;
    int waked_workers = 0;

    while (cursor != NULL) {
        if (cursor->worker_info->type == type) {
            sem_post(&cursor->worker_info->wakeup);
            waked_workers++;
        }
        cursor = cursor->next;
    }
    fprintf(stderr, "Number of waked workers: %d\n", waked_workers);
}

pthread_t create_worker(char *name, workplace_type type) {
    worker_info_t *worker_info = (worker_info_t *) malloc(sizeof(worker_info_t));

    worker_info->name = (char *) malloc((strlen(name) + 3) * sizeof(char));
    strcpy(worker_info->name, name);

    worker_info->type = type;
    worker_info->is_working = false;
    worker_info->is_active = true;
    sem_init(&(worker_info->wakeup), 1, 0);

    pthread_t thread;
    int err = pthread_create(&thread, NULL, worker, (void *) worker_info);
    if (err != 0) {
        fprintf(stderr, "%s\n", strerror(err));
    }

    running_worker_t *running = (running_worker_t *) malloc(sizeof(running_worker_t));
    running->next = NULL;
    running->thread = thread;
    running->worker_info = worker_info;

    if (running_workers_head == NULL) {
        running_workers_head = running;
    } else {
        running_worker_t *cursor = running_workers_head;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }
        cursor->next = running;
    }
    fprintf(stderr, "Worker with name \"%s\" was created and attached to the workplace \"%s\".\n", name,
            get_workplace_name(type));
    return thread;
}

void remove_worker(char *name) {
    running_worker_t *cursor = running_workers_head;
    running_worker_t *previous = cursor;

    while (cursor != NULL) {
        if (strcmp(name, cursor->worker_info->name) == 0) {
            cursor->worker_info->is_active = false;
            sem_post(&cursor->worker_info->wakeup);
            pthread_join(cursor->thread, NULL);

            if (cursor == running_workers_head) {
                running_workers_head = cursor->next;
            } else {
                previous->next = cursor->next;
            }
            cursor->next = NULL;
            free(cursor->worker_info->name);
            cursor->worker_info->name = NULL;
            free(cursor->worker_info);
            cursor->worker_info = NULL;
            free(cursor);
            fprintf(stderr, "Worker with name \"%s\" was disposed.\n", name);
            return;
        }
        previous = cursor;
        cursor = cursor->next;
    }
    fprintf(stderr, "No worker was removed because \"%s\" does not exist!\n", name);
}

void free_workers() {
    fprintf(stderr, "Disposing ALL workers!\n");
    running_worker_t *cursor = running_workers_head;
    while (cursor != NULL) {
        running_worker_t *to_be_freed = cursor;
        cursor = cursor->next;
        char *name = (char *) malloc(sizeof(char) * (strlen(to_be_freed->worker_info->name) + 3));
        strcpy(name, to_be_freed->worker_info->name);
        remove_worker(name);
        free(name);
    }
    fprintf(stderr, "All workers disposed!\n");
}
