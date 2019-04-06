#include "global.h"

#ifndef HW06_WORKPLACE_PROVIDER_H
#define HW06_WORKPLACE_PROVIDER_H

_Bool contains_workplace(workplace_type type);

void delete_workplace(workplace_type type);

void delete_workplace_by_id(int id);

void add_workplace(workplace_type type);

workplace_t *get_workplace(workplace_type type);

char *get_workplace_name(workplace_type type);

workplace_type parse_workplace_type(char *type);

void free_workplaces();

#endif //HW06_WORKPLACE_PROVIDER_H
