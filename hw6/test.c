#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

 typedef enum {
    NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA,
    _PLACE_COUNT, HOTOVO
} place;

typedef enum {
    A, B, C,
    _PRODUCT_COUNT
} product;


 typedef struct worker_{
    int works; //if works
    int ends; //if will end
    place pl; //where works
    char *work_n; //name
    sem_t wakes; //wakeup thread
    
    pthread_t thr;
    struct worker_ *next;
 } worker_st;
 
 worker_st *worker_head = NULL;
 
 typedef struct component{
    useconds_t sleep_time;
    int step;
    product prod;
    place now_pl;
    place prev_pl;
    struct component *next_comp;
 } component_st;
 component_st *component_head = NULL;
 
 typedef struct workpl {
    int works; //if works
    int ends; //if mutex is locked
    place pl;
    pthread_mutex_t mutex;
    int workplace_id;
    struct workpl *next_workpl; //for linked list
 } workplace;
 
workplace *workplace_head = NULL;
 
/* You can use these functions and data structures to transform
 * strings to numbers and use them in arrays
 */

 
const char *place_str[_PLACE_COUNT] = {
    [NUZKY] = "nuzky",
    [VRTACKA] = "vrtacka",
    [OHYBACKA] = "ohybacka",
    [SVARECKA] = "svarecka",
    [LAKOVNA] = "lakovna",
    [SROUBOVAK] = "sroubovak",
    [FREZA] = "freza",
};
 

 
const char *product_str[_PRODUCT_COUNT] = {
    [A] = "A",
    [B] = "B",
    [C] = "C",
};
 
int find_string_in_array(const char **array, int length, char *what)
{
    for (int i = 0; i < length; i++)
	if (strcmp(array[i], what) == 0)
	    return i;
    return -1;
}

 
/* It is not necessary to represent each working place with a dynamically allocated object.
 * You can store only number of ready places
 */
int ready_places[_PLACE_COUNT];

 
/* It is not necessary to represent each part as a dynamically allocated object.
 * you can have only number of parts for each working phase
 */
  #define _PHASE_COUNT 6
  int parts[_PRODUCT_COUNT][_PHASE_COUNT];

 void *do_job(void *arg){
    worker_st *worker = arg;
    fprintf(stderr, "Worker %s started\n", worker->work_n);
    
    while(!worker->ends){
    
    //*****MANAGING WORKER
        workplace *cursor = workplace_head;       
        workplace *w_place = NULL;
        while (cursor != NULL){
            if(cursor->pl == worker->pl){
                if (!cursor->works){
                    w_place = cursor;
                } else {
                    break;
                }
            }
            cursor = cursor->next_workpl;
        }
        //*****WAITING FOR WORKSPACE
        if (w_place == NULL){
            fprintf(stderr, "Worker %s is waiting for workspace\n", worker->work_n);
            sem_wait(&worker->wakes);
            fprintf(stderr, "Worker %s woke up - finded workspace?\n", worker->work_n);
            continue;
        }
        //fprintf(stderr, "workplace %s found\n", place_str[w_place->pl]);
        
        pthread_mutex_lock(&w_place->mutex);
        
        
        //----DOING COMPONENT
        component_st *component = NULL;
        int max_step = -1;
        //CHOOSING GOOD component
        if (worker->pl == NUZKY){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        
        }else if(worker->pl == VRTACKA){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        } else if(worker->pl == OHYBACKA){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        } else if(worker->pl == SVARECKA){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        } else if(worker->pl == LAKOVNA){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        } else if(worker->pl == SROUBOVAK){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        } else if(worker->pl == FREZA){
            component_st *cursor = component_head;
            while (cursor != NULL){
                if (cursor->step > max_step && cursor->now_pl == worker->pl){
                    max_step = cursor->step;
                    component = cursor;
                }
                cursor = cursor->next_comp;             
            }
        }
        
        //---waitin for JOB
        if (component == NULL){
            pthread_mutex_unlock(&w_place->mutex);
            fprintf(stderr, "Worker %s is waiting for job\n", worker->work_n);
            sem_wait(&worker->wakes);
            fprintf(stderr, "Worker %s WOKE UP\n", worker->work_n);
            continue;
        }
        //they are working
        worker->works = 1;
        w_place->works = 1;
        fprintf(stderr, "Worker %s is now working on %s \n", worker->work_n, place_str[w_place->pl]);
        //printin d shit out
        printf("%s %s %s %d\n", worker->work_n, place_str[w_place->pl], product_str[component->prod], component->step);
        
        //lets go sleep baby
        useconds_t sleep_time = component->sleep_time;
        fprintf(stderr, "%s sleeps for %d \n", worker->work_n, sleep_time);
        usleep(sleep_time);
        
        //ITERATING AND SEEKING FOR NEXT/ FUTURE step
        //now iterate through this, cuz we wanna know next workplace where component will be
        place next_pl;
        place A[6] = {NUZKY, VRTACKA, OHYBACKA, SVARECKA, VRTACKA, LAKOVNA};
        place B[6] = {VRTACKA, NUZKY, FREZA, VRTACKA, LAKOVNA, SROUBOVAK};
        place C[6] = {FREZA, VRTACKA, SROUBOVAK, VRTACKA, FREZA, LAKOVNA};
        if (component->step < 6){
            if (!strcmp(product_str[component->prod], "A")){
                if (component->now_pl == NUZKY && component->step == 1){
                    next_pl = VRTACKA;
                }else{
                    for (int i = 1; i < 5; i++){
                        if ((A[i] == component->now_pl )&& component->prev_pl == A[i-1]){
                            next_pl = A[i+1];
                            break;
                        }
                    }
                }
            } else if (!strcmp(product_str[component->prod],"B")){
                if (component->now_pl == VRTACKA && component->step == 1){
                    next_pl = NUZKY;
                } else{
                    for (int i = 1; i < 5; i++){
                        if ((B[i] == component->now_pl )&& component->prev_pl == B[i-1]){
                            next_pl = B[i+1];
                            break;
                        }
                    }
                }
            } else if (!strcmp(product_str[component->prod],"C")){
                if (component->now_pl == FREZA && component->step == 1){
                    next_pl = VRTACKA;
                } else{
                    for (int i = 1; i < 5; i++){
                        if ((C[i] == component->now_pl )&& component->prev_pl == C[i-1]){
                            next_pl = C[i+1];
                            break;
                        }
                    }
                }
            }
        }
        int step_bef;
        //CHECKING IF DONE
        if (!strcmp(product_str[component->prod], "A") && !strcmp(place_str[worker->pl], "lakovna") && (component->step == 6)){
            printf("done %s\n", product_str[component->prod]);
            step_bef = 6;
            component_st *cursor2 = component_head;
            component_st *previous = cursor2;
            
            while (cursor2 != NULL){
                
                if (cursor2->now_pl == worker->pl){
                    
                    if (cursor2 == component_head){
                        component_head = cursor2->next_comp;
                    } else {
                        previous->next_comp = cursor2->next_comp;
                    }
                    cursor2->next_comp = NULL;
                    
                    free(cursor2);
                    break;
                }
                
                previous = cursor2;
                cursor2 = cursor2->next_comp;
            }  
        } else if (!strcmp(product_str[component->prod],"B") && !strcmp(place_str[worker->pl], "sroubovak") && (component->step == 6)){
            printf("done %s\n", product_str[component->prod]);
            step_bef = 6;
                        component_st *cursor2 = component_head;
            component_st *previous = cursor2;
            
            while (cursor2 != NULL){
                
                if (cursor2->now_pl == worker->pl){
                    
                    if (cursor2 == component_head){
                        component_head = cursor2->next_comp;
                    } else {
                        previous->next_comp = cursor2->next_comp;
                    }
                    cursor2->next_comp = NULL;
                    
                    free(cursor2);
                    break;
                }
                
                previous = cursor2;
                cursor2 = cursor2->next_comp;
            }  
        }  else if (!strcmp(product_str[component->prod],"C") && !strcmp(place_str[worker->pl], "lakovna") && (component->step == 6)){
            printf("done %s\n", product_str[component->prod]);
            step_bef = 6;
                        component_st *cursor2 = component_head;
            component_st *previous = cursor2;
            
            while (cursor2 != NULL){
                
                if (cursor2->now_pl == worker->pl){
                    
                    if (cursor2 == component_head){
                        component_head = cursor2->next_comp;
                    } else {
                        previous->next_comp = cursor2->next_comp;
                    }
                    cursor2->next_comp = NULL;
                    
                    free(cursor2);
                    break;
                }
                
                previous = cursor2;
                cursor2 = cursor2->next_comp;
            }  
        } else {
            //IF NOT DONE - DO THIS
            component->prev_pl = component->now_pl;
            fprintf(stderr,"nextplace is %s\n", place_str[next_pl]);
            component->now_pl = next_pl;
            if (component->now_pl == NUZKY){
                component->sleep_time = 100000;
            } else if (component->now_pl == VRTACKA){
                component->sleep_time = 200000;
            } else if (component->now_pl == OHYBACKA){
                component->sleep_time = 150000;
            } else if (component->now_pl == SVARECKA){
                component->sleep_time = 300000;
            } else if (component->now_pl == LAKOVNA){
                component->sleep_time = 400000;
            } else if (component->now_pl == SROUBOVAK){
                component->sleep_time = 250000;
            } else if (component->now_pl == FREZA){
                component->sleep_time = 500000;
            }
            step_bef = component->step;
            component->step = component->step + 1;

        }
        
        //set them sleep - they stop working
        worker->works = 0;
        w_place->works = 0;
        
        pthread_mutex_unlock(&w_place->mutex);
        
//----WAKIN thread if there is job
        if (step_bef < 6){
            worker_st *cursor1 = worker_head;       
            while (cursor1 != NULL){
                if(cursor1->pl == component->now_pl){
                    if (!cursor1->works){
                        sem_post(&cursor1->wakes);
                    } else {
                        break;
                    }
                }
                cursor1 = cursor1->next;
            }
        }
        //DELETING WORKSPACE
        if (w_place->ends){
        
            workplace *cursor = workplace_head;
            workplace *previous = cursor;
            
            while (cursor != NULL){
                
                if (cursor->pl == w_place->pl && cursor->works == 0){
                    pthread_mutex_lock(&cursor->mutex);
                    
                    if (cursor == workplace_head){
                        workplace_head = cursor->next_workpl;
                    } else {
                        previous->next_workpl = cursor->next_workpl;
                    }
                    cursor->next_workpl = NULL;
                    
                    
                    pthread_mutex_unlock(&cursor->mutex);
                    pthread_mutex_destroy(&cursor->mutex);
                    free(cursor);
                    
                    fprintf(stderr, "Workplace %s was deleted\n", place_str[w_place->pl]);
                    break;
                }
                
                previous = cursor;
                cursor = cursor->next_workpl;
            }  
        }
           
    }
    return NULL;
    
 }
 
 
int main(int argc, char **argv){
    /* Initialize your internal structures, mutexes and conddition
     * variables. */
 
    while (1) {
	char *line, *cmd, *arg1, *arg2, *arg3, *saveptr;
	char *worker_n;
	char *workplace_n;
	int s = scanf(" %m[^\n]", &line);
	if (s == EOF)
	    break;
	if (s == 0)
	    continue;
 
	cmd  = strtok_r(line, " ", &saveptr);
	arg1 = strtok_r(NULL, " ", &saveptr);
	arg2 = strtok_r(NULL, " ", &saveptr);
	arg3 = strtok_r(NULL, " ", &saveptr);
 
	if (strcmp(cmd, "start") == 0 && arg1 && arg2 && !arg3) {
	
	//--------------------------------------------------
	
	    //INITIALIZING WORKER
	     worker_n = strdup(arg1);
	     //workplace_n = strdup(arg2);
	     fprintf(stderr, "workpace = %s\n", arg2);
	     worker_st *worker = malloc(sizeof(worker_st));
	     pthread_t thr;
	     worker->work_n = worker_n;
	     worker->ends = 0;
	     worker->works = 0;
	     if (!strcmp(arg2,"nuzky")){
	        worker->pl = NUZKY;
	     } else if(!strcmp(arg2,"vrtacka")){
	        worker->pl = VRTACKA;
	     } else if(!strcmp(arg2,"ohybacka")){
	        worker->pl = OHYBACKA;
	     } else if(!strcmp(arg2,"svarecka")){
	        worker->pl = SVARECKA;
	     } else if(!strcmp(arg2,"lakovna")){
	        worker->pl = LAKOVNA;
	     } else if(!strcmp(arg2,"sroubovak")){
	        worker->pl = SROUBOVAK;
	     } else if(!strcmp(arg2,"freza")){
	        worker->pl = FREZA;
	     }
	     fprintf(stderr, "worker place = %s\n", place_str[worker->pl]);
	     
	     //doing sem
	      sem_init(&(worker->wakes), 1, 0);
	      
	      //*******************------------------------**********
	     //creating thread
	     pthread_create(&thr, NULL, do_job, worker);
	     worker->next = NULL;
	     worker->thr = thr;	     
         if (worker_head == NULL) {
             worker_head = worker;
         } else {
             worker_st *cursor = worker_head;
             while (cursor->next != NULL) {
                 cursor = cursor->next;
             }
             cursor->next = worker;
         }
        fprintf(stderr,"Worker %s was created and his place is %s\n", worker->work_n, place_str[worker->pl]);
	     

	 //ADDING COMPONENT/PART    
	} else if (strcmp(cmd, "make") == 0 && arg1 && !arg2) {
	//fillin structure
	    component_st *comp = malloc(sizeof(component_st));
	    if (!strcmp(arg1,"A")){
	        comp->prod = A;
	        comp->now_pl = NUZKY; 
	        comp->sleep_time = 100000;
	     } else if(!strcmp(arg1,"B")){
	        comp->prod = B;
	        comp->now_pl = VRTACKA; 
	        comp->sleep_time = 200000;
	     } else if(!strcmp(arg1,"C")){
	        comp->prod = C;
	        comp->now_pl = FREZA; 
	        comp->sleep_time = 500000;
	     }
	    comp->step = 1;
	    
	     comp->next_comp = NULL;     
         if (component_head == NULL) {
             component_head = comp;
         } else {
             component_st *cursor = component_head;
             while (cursor->next_comp != NULL) {
                 cursor = cursor->next_comp;
             }
             cursor->next_comp = comp;
         }
        //Comp created
	    fprintf(stderr, "New component %s was created\n", product_str[comp->prod]);
	     
	     //-------WAKING UP WORKERS
        worker_st *cursor = worker_head;
        int waked_workers = 0;

        while (cursor != NULL) {
            if (cursor->pl == comp->now_pl) {
                sem_post(&cursor->wakes);
                waked_workers++;
            }
            cursor = cursor->next;
        }
        fprintf(stderr, "Number of waked workers: %d\n", waked_workers);
	    
	} else if (strcmp(cmd, "end") == 0 && arg1 && !arg2) {
	    /* tell the worker to finish
	     * the worker has to finish their work first
	     * you should not wait here for the worker to finish */
	    /* if the worker is waiting for work
	       you need to wakeup the worker */ 
	        worker_st *cursor = worker_head;
	        worker_st *previous = cursor;
	        
	        while (cursor != NULL){
	            if (!strcmp(arg1, cursor->work_n)){
	                cursor->ends = 1;
	                sem_post(&cursor->wakes);
	                pthread_join(cursor->thr, NULL);
	                
	                if (cursor == worker_head){
	                    worker_head = cursor->next;
	                } else{
	                    previous->next = cursor->next;
	                }
	                cursor->next = NULL;
	                free(cursor->work_n);
	                free(cursor);
	                fprintf(stderr, "Worker %s was deleted", arg1);
	                break;
	            }
	            previous = cursor;
	            cursor = cursor->next;
	        }
        
	} else if (strcmp(cmd, "add") == 0 && arg1 && !arg2) {

        workplace *work_pl = malloc(sizeof(workplace));
	     if (!strcmp(arg1,"nuzky")){
	        work_pl->pl = NUZKY;
	     } else if(!strcmp(arg1,"vrtacka")){
	        work_pl->pl = VRTACKA;
	     } else if(!strcmp(arg1,"ohybacka")){
	        work_pl->pl = OHYBACKA;
	     } else if(!strcmp(arg1,"svarecka")){
	        work_pl->pl = SVARECKA;
	     } else if(!strcmp(arg1,"lakovna")){
	        work_pl->pl = LAKOVNA;
	     } else if(!strcmp(arg1,"sroubovak")){
	        work_pl->pl = SROUBOVAK;
	     } else if(!strcmp(arg1,"freza")){
	        work_pl->pl = FREZA;
	     } 
        work_pl->works = 0;
        work_pl->ends = 0;
        work_pl->next_workpl = NULL;
	    pthread_mutex_init(&(work_pl->mutex), NULL);
	    
        if (workplace_head == NULL) {
            workplace_head = work_pl;
        } else {
            workplace *tail = workplace_head;
            while (tail != NULL) {
                if (tail->next_workpl != NULL) {
                    tail = tail->next_workpl;
                } else {
                    tail->next_workpl = work_pl;
                    break;
                }
            }
        }

        fprintf(stderr, "workplace %s added.\n", place_str[work_pl->pl]);
	    
	    
	} else if (strcmp(cmd, "remove") == 0 && arg1 && !arg2) {
	    /* if you cannot remove empty place you cannot wait for finish work */
            workplace *cursor = workplace_head;
            workplace *previous = cursor;
            
            while (cursor != NULL){
                
                if (!strcmp(place_str[cursor->pl],arg1) && cursor->works == 0){
                    pthread_mutex_lock(&cursor->mutex);
                    
                    if (cursor == workplace_head){
                        workplace_head = cursor->next_workpl;
                    } else {
                        previous->next_workpl = cursor->next_workpl;
                    }
                    cursor->next_workpl = NULL;
                    
                    
                    pthread_mutex_unlock(&cursor->mutex);
                    pthread_mutex_destroy(&cursor->mutex);
                    free(cursor);
                    
                    fprintf(stderr, "Workplace %s was deleted\n", arg1);
                    break;
                }
                
                previous = cursor;
                cursor = cursor->next_workpl;
            } 
	} else {
	    fprintf(stderr, "Invalid command: %s\n", line);
	}
	free(line);
    }
    
    //free worker
    fprintf(stderr, "freeing all workers\n");
    worker_st *cursor = worker_head;
    while (cursor != NULL){
        worker_st *will_free = cursor;
        cursor = cursor->next;
        will_free->ends = 1;
        sem_post(&will_free->wakes);
        pthread_join(will_free->thr, NULL);
        
        if (will_free == worker_head){
            worker_head = cursor;
        }
        free(will_free->work_n);
        free(will_free);
        fprintf(stderr, "Worker was freed\n");

    }
    //free workplaces
    workplace *cursor3 = workplace_head;
    fprintf(stderr, "freeing workplaces\n");
    while (cursor3 != NULL){
            workplace *free_ = cursor3;
            cursor3 = free_->next_workpl;
            
            pthread_mutex_lock(&free_->mutex);
            pthread_mutex_unlock(&free_->mutex);
            pthread_mutex_destroy(&free_->mutex);
            free(free_);
            fprintf(stderr, "workpl was freed\n");
    } 
    //free all components
    fprintf(stderr,"freeing comps\n");
     component_st *curs =  component_head;
     while(curs != NULL){
        component_st *will_fr = curs;
        curs = curs->next_comp;
        if (will_fr = component_head){
            component_head = curs;
        }
        free(will_fr);
        fprintf(stderr, "Comp was freed\n");
     }
     
    return 0;
 
    /* Wait for every worker to finish their work. Nobody should be
     * able to continue. */
}


