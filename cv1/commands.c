
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "job_provider.h"
#include "worker_provider.h"
#include "workplace_provider.h"

_Bool command_make() {
    char char_type;
    if (scanf(" %c", &char_type) != 1) {
        fprintf(stderr, "Reading error!\n");
        return false;
    }
    fprintf(stderr, "Command MAKE for \'%c\'.\n", char_type);
    job_t *job = create_job(char_type);
    add_job(job);
    return true;
}


_Bool command_start() {
    char *worker_name, *workplace_name;
    if (scanf("%ms", &worker_name) != 1) {
        fprintf(stderr, "Reading error!\n");
        if (worker_name != NULL) {
            free(worker_name);
        }
        return false;
    }

    if (scanf("%ms", &workplace_name) != 1) {
        fprintf(stderr, "Reading error!\n");
        if (workplace_name != NULL) {
            free(workplace_name);
        }
        return false;
    }
    fprintf(stderr, "Command START for \"%s\" - \"%s\".\n", worker_name, workplace_name);
    workplace_type type = parse_workplace_type(workplace_name);

    if ((int) type != -1) {
        create_worker(worker_name, type);
    }

    free(worker_name);
    free(workplace_name);

//    sem_wait(&producer_wake);
    return true;

}

_Bool command_end() {
    char *worker_name;
    if (scanf("%ms", &worker_name) != 1) {
        fprintf(stderr, "Reading error!\n");
        if (worker_name != NULL) {
            free(worker_name);
        }
        return false;
    }
    fprintf(stderr, "Command END for \"%s\".\n", worker_name);
    remove_worker(worker_name);
    free(worker_name);
    return true;
}

_Bool command_add() {
    char *workplace_name;
    if (scanf("%ms", &workplace_name) != 1) {
        fprintf(stderr, "Reading error!\n");
        if (workplace_name != NULL) {
            free(workplace_name);
        }
        return false;
    }

    fprintf(stderr, "Command ADD for \"%s\".\n", workplace_name);
    workplace_type type = parse_workplace_type(workplace_name);

    if ((int) type != -1) {
        add_workplace(type);
    }

    free(workplace_name);
    return true;
}

_Bool command_remove() {
    char *workplace_name;
    if (scanf("%ms", &workplace_name) != 1) {
        fprintf(stderr, "Reading error!\n");
        if (workplace_name != NULL) {
            free(workplace_name);
        }
        return false;
    }
    fprintf(stderr, "Command REMOVE for \"%s\".\n", workplace_name);
    workplace_type type = parse_workplace_type(workplace_name);

    if ((int) type != -1) {
        delete_workplace(type);
    }

    free(workplace_name);
    return true;
}
