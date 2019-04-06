
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <stdio.h>
#include "workplace_provider.h"

workplace_t *workplaces_head = NULL;

_Bool contains_workplace(workplace_type type) {
    return get_workplace(type) != NULL;
}

char *get_workplace_name(workplace_type type) {
    switch (type) {
        case SCISSORS:
            return "nuzky";
        case DRILL:
            return "vrtacka";
        case BENDING_MACHINE:
            return "ohybacka";
        case WELDER:
            return "svarecka";
        case PAINTER:
            return "lakovna";
        case SCREWDRIVER:
            return "sroubovak";
        case MILLING_CUTTER:
            return "freza";
        default:
            return "unsupported type";
    }
}

workplace_t *get_workplace(workplace_type type) {
    workplace_t *cursor = workplaces_head;
    workplace_t *possible_workplace = NULL;

    while (cursor != NULL) {
        if (cursor->type == type) {
            if (cursor->is_working) {
                possible_workplace = cursor;
            } else {
                return cursor;
            }
        }

        cursor = cursor->next_workplace;
    }

    return possible_workplace;
}

workplace_type parse_workplace_type(char *type) {
    if (strcmp("nuzky", type) == 0) {
        return SCISSORS;

    } else if (strcmp("vrtacka", type) == 0) {
        return (DRILL);

    } else if (strcmp("ohybacka", type) == 0) {
        return (BENDING_MACHINE);

    } else if (strcmp("svarecka", type) == 0) {
        return (WELDER);

    } else if (strcmp("lakovna", type) == 0) {
        return (PAINTER);

    } else if (strcmp("sroubovak", type) == 0) {
        return (SCREWDRIVER);

    } else if (strcmp("freza", type) == 0) {
        return (MILLING_CUTTER);

    }

    fprintf(stderr, "Workplace \"%s\" is not defined!\n", type);
    return (workplace_type) -1;
}

void delete_workplace_by_id(int id) {
    workplace_t *cursor = workplaces_head;
    workplace_t *previous = cursor;

    while (cursor != NULL) {
        if (cursor->workplace_id == id) {
            pthread_mutex_lock(&cursor->mutex);

            if (cursor == workplaces_head) {
                workplaces_head = cursor->next_workplace;
            } else {
                previous->next_workplace = cursor->next_workplace;
            }

            cursor->next_workplace = NULL;

            pthread_mutex_unlock(&cursor->mutex);
            pthread_mutex_destroy(&cursor->mutex);
            free(cursor);

            fprintf(stderr, "Workplace [Id: %d] was disposed. This workplace was not working\n", id);
            return;
        }

        previous = cursor;
        cursor = cursor->next_workplace;
    }
}

void delete_workplace(workplace_type type) {
    fprintf(stderr, "Workplace \"%s\" will be removed.\n", get_workplace_name(type));

    workplace_t *cursor = workplaces_head;
    workplace_t *previous = cursor;

    while (cursor != NULL) {
        if (cursor->type == type && !cursor->is_working) {
            pthread_mutex_lock(&cursor->mutex);

            if (cursor == workplaces_head) {
                workplaces_head = cursor->next_workplace;
            } else {
                previous->next_workplace = cursor->next_workplace;
            }

            cursor->next_workplace = NULL;

            pthread_mutex_unlock(&cursor->mutex);
            pthread_mutex_destroy(&cursor->mutex);
            free(cursor);

            fprintf(stderr, "Workplace \"%s\" was disposed. This workplace was not working\n",
                    get_workplace_name(type));
            return;
        }

        previous = cursor;
        cursor = cursor->next_workplace;
    }
    fprintf(stderr, "\n HERREEEEE\n");

    cursor = workplaces_head;
    while (cursor != NULL) {
        if (cursor->type == type) {
            cursor->is_active = false;
            fprintf(stderr, "Workplace \"%s\" [Id: %d] will be disposed. This workplace was working\n", get_workplace_name(type), cursor->workplace_id);
            return;
        }

        cursor = cursor->next_workplace;
    }

    fprintf(stderr, "Workplace \"%s\" was not found!.\n", get_workplace_name(type));
}

static int id_counter = 0;

void add_workplace(workplace_type type) {
    fprintf(stderr, "Adding new workplace \"%s\".\n", get_workplace_name(type));

    workplace_t *new_workplace = (workplace_t *) malloc(sizeof(workplace_t));
    new_workplace->type = type;
    new_workplace->is_working = false;
    new_workplace->next_workplace = NULL;
    new_workplace->workplace_id = id_counter++;
    pthread_mutex_init(&(new_workplace->mutex), NULL);

    if (workplaces_head == NULL) {
        workplaces_head = new_workplace;
    } else {
        workplace_t *tail = workplaces_head;
        while (tail != NULL) {
            if (tail->next_workplace != NULL) {
                tail = tail->next_workplace;
            } else {
                tail->next_workplace = new_workplace;
                break;
            }
        }
    }

    fprintf(stderr, "workplace \"%s\" added.\n", get_workplace_name(type));
}

void free_workplaces() {
    workplace_t *cursor = workplaces_head;

    while (cursor != NULL) {
        workplace_t *current = cursor;
        cursor = current->next_workplace;

        pthread_mutex_lock(&current->mutex); //this will waits for another job, which is taking place on this workplace
        pthread_mutex_unlock(&current->mutex);
        pthread_mutex_destroy(&current->mutex);
        free(current);
    }
}
