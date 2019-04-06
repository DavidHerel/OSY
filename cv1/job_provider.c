
#include <stdlib.h>
#include <stdio.h>
#include "job_provider.h"
#include "buffers.h"
#include "workplace_chain.h"
#include "worker_provider.h"
#include "workplace_provider.h"

char get_job_type_name(job_type type) {
    switch (type) {

        case A:
            return 'A';
        case B:
            return 'B';
        case C:
            return 'C';
        default:
            break;
    }

    fprintf(stderr, "Wrong job_type! \'%d\' is nod valid job type.\n", type);
    return 'N'; //not defined
}

job_type get_job_type(char type) {
    switch (type) {
        case 'A':
            return A;
        case 'B':
            return B;
        case 'C':
            return C;
        default:
            break;
    }
    fprintf(stderr, "Wrong char for job type! \'%c\' is nod valid job type.\n", type);
    return (job_type) -1; //not defined
}

job_t *create_job(char char_type) {
    job_t *job = (job_t *) malloc(sizeof(job_t));
    job->type = get_job_type(char_type);;
    job->current_workplace = NOT_STARTED;
    job->previous_workplace = NOT_STARTED;
    job->step = 1;

    workplace_type place = get_next_workplace_type(job);
    __useconds_t sleep_time = get_sleep_time(place);

    job->sleep_time = sleep_time;
    job->current_workplace = place;
    fprintf(stderr, "New job \'%c\' was created.\n", char_type);
    return job;
}

void add_job(job_t *job_to_add) {
    fprintf(stderr, "Adding job to \"%s\".\n", get_workplace_name(job_to_add->current_workplace));
    switch (job_to_add->current_workplace) {
        case SCISSORS:
            add_scissors_job(job_to_add);
            break;
        case DRILL:
            add_drill_job(job_to_add);
            break;
        case BENDING_MACHINE:
            add_bending_machine_job(job_to_add);
            break;
        case WELDER:
            add_welder_job(job_to_add);
            break;
        case PAINTER:
            add_painter_job(job_to_add);
            break;
        case SCREWDRIVER:
            add_screwdriver_job(job_to_add);
            break;
        case MILLING_CUTTER:
            add_milling_job(job_to_add);
        default:
            break;
    }
    wake_up_workers(job_to_add->current_workplace);
    fprintf(stderr, "add JOB ONLY one\n");
}

job_t *get_job(workplace_type workplace) {
    switch (workplace) {
        case SCISSORS:
            return get_scissors_job();
        case DRILL:
            return get_drill_job();
        case BENDING_MACHINE:
            return get_bending_machine_job();
        case WELDER:
            return get_welder_job();
        case PAINTER:
            return get_painter_job();
        case SCREWDRIVER:
            return get_screwdriver_job();
        case MILLING_CUTTER:
            return get_milling_job();
        default:
            return NULL;
    }
    return NULL;
}
