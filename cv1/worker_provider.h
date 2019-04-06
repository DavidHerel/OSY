#ifndef HW06_WORKER_PROVIDER_H
#define HW06_WORKER_PROVIDER_H

#include <pthread.h>
#include "global.h"

_Bool is_somebody_working();

_Bool contains_worker(workplace_type type);

pthread_t create_worker(char *name, workplace_type type);

void remove_worker(char *name);

void wake_up_workers(workplace_type type);

void free_workers();

#endif //HW06_WORKER_PROVIDER_H
