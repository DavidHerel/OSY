
#ifndef HW06_WORKPLACE_CHAIN_H
#define HW06_WORKPLACE_CHAIN_H

#include "global.h"

workplace_type get_next_workplace_type(job_t *job);
__useconds_t get_sleep_time(workplace_type current_workplace);

#endif //HW06_WORKPLACE_CHAIN_H
