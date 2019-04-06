
#include "global.h"

/*
A: 1:nuzky   - 2:vrtacka - 3:ohybacka  - 4:svarecka - 5:vrtacka - 6:lakovna
B: 1:vrtacka - 2:nuzky   - 3:freza     - 4:vrtacka  - 5:lakovna - 6:sroubovak
C: 1:freza   - 2:vrtacka - 3:sroubovak - 4:vrtacka  - 5:freza   - 6:lakovna
*/

workplace_type A_chain[8] = {NOT_STARTED, SCISSORS, DRILL, BENDING_MACHINE, WELDER, DRILL, PAINTER, FINISHED};
workplace_type B_chain[8] = {NOT_STARTED, DRILL, SCISSORS, MILLING_CUTTER, DRILL, PAINTER, SCREWDRIVER, FINISHED};
workplace_type C_chain[8] = {NOT_STARTED, MILLING_CUTTER, DRILL, SCREWDRIVER, DRILL, MILLING_CUTTER, PAINTER, FINISHED};


workplace_type
get_next(workplace_type previous_workplace, workplace_type current_workplace, workplace_type *chain, int chain_size) {
    if (current_workplace == NOT_STARTED) {
        return chain[1];
    }

    for (int i = 1; i < chain_size; i++) {
        if (current_workplace == chain[i] && previous_workplace == chain[i - 1]) {
            workplace_type result = ++i >= chain_size ? FINISHED : chain[i];
            return result;
        }
    }

    return FINISHED;
}


workplace_type get_next_workplace_type(job_t *job) {
    switch (job->type) {
        case A:
            return get_next(job->previous_workplace, job->current_workplace, A_chain, 8);
        case B:
            return get_next(job->previous_workplace, job->current_workplace, B_chain, 8);
        case C:
            return get_next(job->previous_workplace, job->current_workplace, C_chain, 8);
        default:
            return FINISHED;
    }
}


/*
nuzky: 100
vrtacka: 200
ohybacka: 150
svarecka: 300
lakovna: 400
sroubovak: 250
freza: 500
*/

__useconds_t get_sleep_time(workplace_type current_workplace) {
    switch (current_workplace) {
        case SCISSORS:
            return 100000;
        case DRILL:
            return 200000;
        case BENDING_MACHINE:
            return 150000;
        case WELDER:
            return 300000;
        case PAINTER:
            return 400000;
        case SCREWDRIVER:
            return 250000;
        case MILLING_CUTTER:
            return 500000;
        default:
            return 0;
    }
}
