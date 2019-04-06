#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#include "buffers.h"

typedef struct link_list {
    job_t *value;
    struct link_list *next;
} link_list_t;

link_list_t *scissors_head = NULL;
pthread_mutex_t scissors_mutex;

link_list_t *drill_head = NULL;
pthread_mutex_t drill_mutex;

link_list_t *bending_machine_head = NULL;
pthread_mutex_t bending_machine_mutex;

link_list_t *welder_head = NULL;
pthread_mutex_t welder_mutex;

link_list_t *painter_head = NULL;
pthread_mutex_t painter_mutex;

link_list_t *screwdriver_head = NULL;
pthread_mutex_t screwdriver_mutex;

link_list_t *milling_cutter_head = NULL;
pthread_mutex_t milling_cutter_mutex;


_Bool contains_job_in_stage(workplace_type workplace_type) {
    switch (workplace_type) {
        case SCISSORS:
            return scissors_head != NULL;
        case DRILL:
            return drill_head != NULL;
        case BENDING_MACHINE:
            return bending_machine_head != NULL;
        case WELDER:
            return welder_head != NULL;
        case PAINTER:
            return painter_head != NULL;
        case SCREWDRIVER:
            return screwdriver_head != NULL;
        case MILLING_CUTTER:
            return milling_cutter_head != NULL;
        default:
            return false;
    }
}

void free_one_buffer(link_list_t *head) {
    while (head != NULL) {
        link_list_t *to_free = head;
        free(to_free->value);
        head = head->next;
        free(to_free);
    }
}

void free_buffers() {
    free_one_buffer(scissors_head);
    free_one_buffer(drill_head);
    free_one_buffer(bending_machine_head);
    free_one_buffer(welder_head);
    free_one_buffer(painter_head);
    free_one_buffer(screwdriver_head);
    free_one_buffer(milling_cutter_head);

    scissors_head = NULL;
    drill_head = NULL;
    bending_machine_head = NULL;
    welder_head = NULL;
    painter_head = NULL;
    screwdriver_head = NULL;
    milling_cutter_head = NULL;
}

void buffers_init() {
    pthread_mutex_init(&scissors_mutex, NULL);
    pthread_mutex_init(&drill_mutex, NULL);
    pthread_mutex_init(&bending_machine_mutex, NULL);
    pthread_mutex_init(&welder_mutex, NULL);
    pthread_mutex_init(&painter_mutex, NULL);
    pthread_mutex_init(&screwdriver_mutex, NULL);
    pthread_mutex_init(&milling_cutter_mutex, NULL);
}

job_t *get_generic_job(link_list_t **head, pthread_mutex_t *mutex) {
    if (*head == NULL) {
        return NULL;
    }

    pthread_mutex_lock(mutex);

    link_list_t *cursor = *head;
    link_list_t *previous = *head;
    link_list_t *result = (*head);

    while (cursor->next != NULL) {
        if (result->value->step < cursor->next->value->step) {
            result = cursor->next;
            previous = cursor;
        } else if (result->value->step == cursor->next->value->step) {
            if (result->value->type > cursor->next->value->type) {
                result = cursor->next;
                previous = cursor;
            }

        }
        cursor = cursor->next;
    }

    if (result == (*head)) {
        (*head) = (*head)->next;
    } else if (previous == *head) {
        (*head)->next = result->next;
    } else {
        previous->next = result->next;
    }

    job_t *return_value = result->value;
    free(result);
    pthread_mutex_unlock(mutex);
    return return_value;
}

void add_generic_job(link_list_t **head, job_t *job_to_add, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);

    if (*head == NULL) {
        (*head) = (link_list_t*) malloc(sizeof(link_list_t));
        (*head)->value = job_to_add;
        (*head)->next = NULL;
    } else {
        link_list_t *cursor = *head;
        while (cursor->next != NULL) {
            cursor = cursor->next;
        }
        if (cursor == *head) {
            link_list_t * next = (link_list_t*)malloc(sizeof(link_list_t));
            next->next = NULL;
            next->value = job_to_add;
            (*head)->next = next;
        } else {
            link_list_t * next = (link_list_t*)malloc(sizeof(link_list_t));
            next->next = NULL;
            next->value = job_to_add;
            cursor->next = next;
        }
    }

    pthread_mutex_unlock(mutex);
}


job_t *get_scissors_job() {
    return get_generic_job(&scissors_head, &scissors_mutex);
}

void add_scissors_job(job_t *job_to_add) {
    add_generic_job(&scissors_head, job_to_add, &scissors_mutex);
}

job_t *get_drill_job() {
    job_t *result = get_generic_job(&drill_head, &drill_mutex);
    return result;
}

void add_drill_job(job_t *job_to_add) {
    add_generic_job(&drill_head, job_to_add, &drill_mutex);
}

job_t *get_bending_machine_job() {
    return get_generic_job(&bending_machine_head, &bending_machine_mutex);
}

void add_bending_machine_job(job_t *job_to_add) {
    add_generic_job(&bending_machine_head, job_to_add, &bending_machine_mutex);
}

job_t *get_welder_job() {
    return get_generic_job(&welder_head, &welder_mutex);
}

void add_welder_job(job_t *job_to_add) {
    add_generic_job(&welder_head, job_to_add, &welder_mutex);
}

job_t *get_painter_job() {
    return get_generic_job(&painter_head, &painter_mutex);
}

void add_painter_job(job_t *job_to_add) {
    add_generic_job(&painter_head, job_to_add, &painter_mutex);
}

job_t *get_screwdriver_job() {
    return get_generic_job(&screwdriver_head, &screwdriver_mutex);
}

void add_screwdriver_job(job_t *job_to_add) {
    add_generic_job(&screwdriver_head, job_to_add, &screwdriver_mutex);
}

job_t *get_milling_job() {
    return get_generic_job(&milling_cutter_head, &milling_cutter_mutex);
}

void add_milling_job(job_t *job_to_add) {
    add_generic_job(&milling_cutter_head, job_to_add, &milling_cutter_mutex);
}
