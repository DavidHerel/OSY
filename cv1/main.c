#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>

#include "global.h"
#include "buffers.h"
#include "commands.h"
#include "workplace_provider.h"
#include "worker_provider.h"

workplace_type
        options[7] = {SCISSORS, DRILL, BENDING_MACHINE, WELDER, PAINTER, SCREWDRIVER,
                      MILLING_CUTTER};

_Bool can_work() {
    if (is_somebody_working()) {
        return true;
    }
    for (int i = 0; i < 7; i++) {
        workplace_type type = options[i];

        _Bool job = contains_job_in_stage(type);
        if (job) {
            _Bool workplace = contains_workplace(type);
            _Bool worker = contains_worker(type);
            if (worker && workplace) {
                fprintf(stderr, "There is job, worker and workplace for \'%s\'.\n", get_workplace_name(type));
                return true;
            } else {
                fprintf(stderr, "There is no job for \'%s\' - workplace - %d, worker - %d\n", get_workplace_name(type),
                        workplace, worker);
            }
        }
    }

    return false;
}

int main() {
    buffers_init();
    sem_init(&producer_wake, 1, 0);

    int scan_result;
    char *word;
    scan_result = scanf("%ms", &word);
    while (true) {
        while (scan_result == 1) {

            if (strcmp(word, "make") == 0) {
                command_make();
            } else if (strcmp(word, "start") == 0) {
                command_start();
            } else if (strcmp(word, "end") == 0) {
                command_end();
            } else if (strcmp(word, "add") == 0) {
                command_add();
            } else if (strcmp(word, "remove") == 0) {
                command_remove();
            }
            free(word);
            word = NULL;
            scan_result = scanf("%ms", &word);
        }

        if (word != NULL) {
            free(word);
            word = NULL;
        }

        if (scan_result != EOF) {
            continue;
        }
        break;
    }

    fprintf(stderr, "Received EOF, ending application!\n");

    while (can_work()) {
        sem_wait(&producer_wake);
    }

    free_workers();
    free_buffers();
    free_workplaces();
    return 0;
}
