
#ifndef HW06_JOB_PROVIDER_H
#define HW06_JOB_PROVIDER_H

#include "global.h"


void add_job(job_t *job_to_add) ;
job_t * create_job(char type);
job_t * get_job(workplace_type name);
char get_job_type_name(job_type type);

#endif //HW06_JOB_PROVIDER_H


