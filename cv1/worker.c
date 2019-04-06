
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

#include "worker.h"
#include "global.h"
#include "job_provider.h"
#include "workplace_provider.h"
#include "workplace_chain.h"

void *worker(void *arg) {
    worker_info_t *worker_info = (worker_info_t *) arg;
    fprintf(stderr, "Worker \"%s\" started!\n", worker_info->name);
    sem_post(&producer_wake);

    while (worker_info->is_active) {
        fprintf(stderr, "Worker \"%s\" is looking for workplace type \"%s\".\n", worker_info->name,
                get_workplace_name(worker_info->type));
        workplace_t *workplace = get_workplace(worker_info->type);
        if (workplace == NULL) {
            fprintf(stderr, "Worker \"%s\" is waiting for workspace!\n", worker_info->name);
            sem_wait(&worker_info->wakeup);
            fprintf(stderr, "Worker \"%s\" woke up!\n", worker_info->name);
            continue;
        }
        pthread_mutex_lock(&workplace->mutex);

        job_t *job = get_job(worker_info->type);
        if (job == NULL) {
            pthread_mutex_unlock(&workplace->mutex);
            fprintf(stderr, "Worker \"%s\" is waiting for job!\n", worker_info->name);
            sem_wait(&worker_info->wakeup);
            fprintf(stderr, "Worker \"%s\" woke up!\n", worker_info->name);
            continue;
        }

        worker_info->is_working = true;
        workplace->is_working = true;
        fprintf(stderr, "Worker \"%s\" is now working on \"%s\"!\n", worker_info->name,
                get_workplace_name(worker_info->type));

        printf("%s %s %c %d\n", worker_info->name, get_workplace_name(workplace->type), get_job_type_name(job->type),
               job->step);
        useconds_t sleep_time = job->sleep_time;
        fprintf(stderr, "\"%s\" sleeping for %d, ms\n", worker_info->name, sleep_time);
        usleep(sleep_time);

        workplace_type next_workplace_type = get_next_workplace_type(job);
        if (next_workplace_type == FINISHED) {
            printf("done %c\n", get_job_type_name(job->type));
            free(job);
        } else {
            job->previous_workplace = job->current_workplace;
            job->current_workplace = next_workplace_type;
            job->sleep_time = get_sleep_time(next_workplace_type);
            job->step++;
            fprintf(stderr, "%s ACTUALIZE workplace\n", worker_info->name);
            add_job(job);
        }

        worker_info->is_working = false;
        workplace->is_working = false;
        pthread_mutex_unlock(&workplace->mutex);

        if (!workplace->is_active) {
            delete_workplace_by_id(workplace->workplace_id);
        }

        if (!can_work()) {
            sem_post(&producer_wake);
            fprintf(stderr, "No work, waking up main thread.\n");
        }
    }
    sem_post(&producer_wake);
    return NULL;
}
