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

int scanned_EOF = 0;
 typedef struct worker_{
    char *work_n; //name
    sem_t wakes; //wakeup thread
    int works; //if works
    int ends; //if will end
    place pl; //where works
    pthread_t thr;
    struct worker_ *other;
 } worker_st;
 
 worker_st *worker_head = NULL;
 
 typedef struct component{
    place prev_pl;
    struct component *other;
    useconds_t sleep_time;
    int step;
    product prod;
    place now_pl;
 } component_st;
 component_st *component_head = NULL;
 
 typedef struct workpl {
    struct workpl *other; 
    int works; //if works
    int ends; //if mutex is locked
    place pl;
    pthread_mutex_t mutex;
 } workplace;
 
workplace *workplace_head = NULL;
 
/* You can use these functions and data structures to transform
 * strings to numbers and use them in arrays
 */
pthread_mutex_t main_m;
sem_t main_wake;
 
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
place all[7] = {NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA};
int is_there_work(){
    worker_st *crs1 = worker_head;
    while (crs1 != NULL){
        if (crs1->works){
            return 1;
        }
        crs1 = crs1->other;
    }
    for (int i =0; i < 7 ; i++){
        //finding comp to fit this
        component_st *crs = component_head;
            while (crs != NULL){
                if (crs->now_pl == all[i]){
                    break;
                }
                crs = crs->other;             
            }
        //finded
        if (crs != NULL){
            int place_is = 0;
            int work_is = 0;
            workplace *crs2 = workplace_head;       
            workplace *w_place = NULL;
            while (crs2 != NULL){
                if(crs2->pl == crs->now_pl){
                    if (crs2->works){
                        w_place = crs2;
                    } else {
                        w_place = crs2;
                        break;
                    }
                }
                crs2 = crs2->other;
            }
            if (w_place != NULL){
                place_is = 1;
            }
            worker_st *crs1 = worker_head;
            while (crs1 != NULL){
                if (crs1->pl == crs->now_pl){
                    work_is = 1;
                    break;
                }
                crs1 = crs1->other;

            }
            if (work_is && place_is){
                return 1;
            }
            
        }
    }
    
    return 0;
}

int wake_them(){
    worker_st *crs1 = worker_head;
    while (crs1 != NULL){
        if (crs1->works){
            return 1;
        }
        crs1 = crs1->other;
    }
    for (int i =0; i < 7 ; i++){
        //finding comp to fit this
        component_st *crs = component_head;
            while (crs != NULL){
                if (crs->now_pl == all[i]){
                    break;
                }
                crs = crs->other;             
            }
        //finded
        if (crs != NULL){
            int place_is = 0;
            int work_is = 0;
            workplace *crs2 = workplace_head;       
            workplace *w_place = NULL;
            while (crs2 != NULL){
                if(crs2->pl == crs->now_pl){
                    if (crs2->works){
                        w_place = crs2;
                    } else {
                        w_place = crs2;
                        break;
                    }
                }
                crs2 = crs2->other;
            }
            if (w_place != NULL){
                place_is = 1;
            }
            worker_st *crs1 = worker_head;
            while (crs1 != NULL){
                if (crs1->pl == crs->now_pl){
                    work_is = 1;
                    break;
                }
                crs1 = crs1->other;

            }
            if (work_is && place_is){
                sem_post(&crs1->wakes);
                return 1;
            }
            
        }
    }
    
    return 0;
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
    
    sem_post(&main_wake);
    //sem_wait(&worker->wakes);
    while(!worker->ends){
        if (scanned_EOF){
        //*****MANAGING WORKER
            workplace *crs = workplace_head;       
            workplace *w_place = NULL;
            while (crs != NULL){
                if(crs->pl == worker->pl){
                    if (!crs->works){
                        w_place = crs;
                    } else {
                        break;
                    }
                }
                crs = crs->other;
            }
            //*****WAITING FOR WORKSPACE
            if (w_place == NULL){
                //fprintf(stderr, "%s waits for workspaces", worker->work_n);
                sem_wait(&worker->wakes);
                continue;
            }
            
            pthread_mutex_lock(&w_place->mutex);
            pthread_mutex_lock(&main_m);
            
            
            //----DOING COMPONENT
            component_st *component = NULL;
            int max_step = -1;
            //CHOOSING GOOD component
            
            if (worker->pl == NUZKY){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            
            }else if(worker->pl == VRTACKA){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            } else if(worker->pl == OHYBACKA){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            } else if(worker->pl == SVARECKA){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            } else if(worker->pl == LAKOVNA){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            } else if(worker->pl == SROUBOVAK){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            } else if(worker->pl == FREZA){
                component_st *crs = component_head;
                while (crs != NULL){
                    if (crs->step > max_step && crs->now_pl == worker->pl){
                        max_step = crs->step;
                        component = crs;
                    }
                    crs = crs->other;             
                }
            }
            
            //---waitin for JOB
            if (component == NULL){
               // fprintf(stderr, "%s waits for job\n", worker->work_n);
                pthread_mutex_unlock(&main_m);
                pthread_mutex_unlock(&w_place->mutex);
                sem_wait(&worker->wakes);
                continue;
            }
            //fprintf(stderr,"WORKER %s started on  %s \n", worker->work_n, place_str[worker->pl]);
            //they are working
            worker->works = 1;
            w_place->works = 1;
            //printin d shit out
            printf("%s %s %d %s\n", worker->work_n, place_str[w_place->pl], component->step ,product_str[component->prod]);
            
            //lets go sleep baby
            useconds_t sleep_time = component->sleep_time;
            usleep(sleep_time);
            
            //ITERATING AND SEEKING FOR NEXT/ FUTURE step
            //now iterate through this, cuz we wanna know next workplace where component will be
            place next_pl = VRTACKA;
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
           // int step_bef;
            //CHECKING IF DONE
            if (!strcmp(product_str[component->prod], "A") && !strcmp(place_str[worker->pl], "lakovna") && (component->step == 6)){
                printf("done %s\n", product_str[component->prod]);
               // step_bef = 6;
                component_st *crs2 = component_head;
                component_st *prv = crs2;
                
                while (crs2 != NULL){
                    
                    if (crs2->now_pl == worker->pl){
                        
                        if (crs2 == component_head){
                            component_head = crs2->other;
                        } else {
                            prv->other = crs2->other;
                        }
                        crs2->other = NULL;
                        
                        free(crs2);
                        break;
                    }
                    
                    prv = crs2;
                    crs2 = crs2->other;
                }  
            } else if (!strcmp(product_str[component->prod],"B") && !strcmp(place_str[worker->pl], "sroubovak") && (component->step == 6)){
                printf("done %s\n", product_str[component->prod]);
              //  step_bef = 6;
                            component_st *crs2 = component_head;
                component_st *prv = crs2;
                
                while (crs2 != NULL){
                    
                    if (crs2->now_pl == worker->pl){
                        
                        if (crs2 == component_head){
                            component_head = crs2->other;
                        } else {
                            prv->other = crs2->other;
                        }
                        crs2->other = NULL;
                        
                        free(crs2);
                        break;
                    }
                    
                    prv = crs2;
                    crs2 = crs2->other;
                }  
            }  else if (!strcmp(product_str[component->prod],"C") && !strcmp(place_str[worker->pl], "lakovna") && (component->step == 6)){
                printf("done %s\n", product_str[component->prod]);
              //  step_bef = 6;
                            component_st *crs2 = component_head;
                component_st *prv = crs2;
                
                while (crs2 != NULL){
                    
                    if (crs2->now_pl == worker->pl){
                        
                        if (crs2 == component_head){
                            component_head = crs2->other;
                        } else {
                            prv->other = crs2->other;
                        }
                        crs2->other = NULL;
                        
                        free(crs2);
                        break;
                    }
                    
                    prv = crs2;
                    crs2 = crs2->other;
                }  
            } else {
                //IF NOT DONE - DO THIS
                component->prev_pl = component->now_pl;
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
               // step_bef = component->step;
                component->step = component->step + 1;

            }
            
            //set them sleep - they stop working
            worker->works = 0;
            w_place->works = 0;
            pthread_mutex_unlock(&main_m);
            pthread_mutex_unlock(&w_place->mutex);
            //printf("k\n");
    //----WAKIN thread if there is job


            
            
            //DELETING WORKSPACE
            
                workplace *crs12 = workplace_head;
                workplace *prv = crs;
                
                while (crs12 != NULL){
                    if (crs12->ends == 1){
                     //   fprintf(stderr, "zde ma koncit\n");
                        if (crs12->pl == w_place->pl && crs12->works == 0){
                            pthread_mutex_lock(&crs12->mutex);
                            
                            if (crs12 == workplace_head){
                                workplace_head = crs12->other;
                            } else {
                                prv->other = crs12->other;
                            }
                            crs12->other = NULL;
                            
                          //  fprintf(stderr, "deleting workspace\n");
                            pthread_mutex_unlock(&crs12->mutex);
                            pthread_mutex_destroy(&crs12->mutex);
                            free(crs12);
                            
                            break;
                        }
                    }
                    
                    prv = crs12;
                    crs12 = crs12->other;
                }  

            
            if (is_there_work() == 0){   
               // fprintf(stderr, "%s woke MAIN THREAD\n", worker->work_n);
                sem_post(&main_wake);
            }
        }
          
           
    }
    fprintf(stderr, "OUT OF CYCLE\n");
    sem_post(&main_wake);
    return NULL;
    
 }
 
 
int main(int argc, char **argv){
    /* Initialize your internal structures, mutexes and conddition
     * variables. */
     sem_init(&main_wake, 1, 0);
     
     //TODO adding sem to wakeup main thread cuz it will be waiting till workers do job
    pthread_mutex_init(&main_m, NULL);
    while (1) {
	char *line, *cmd, *arg1, *arg2, *arg3, *saveptr;
	char *worker_n;
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
	     
	     //doing sem
	      sem_init(&(worker->wakes), 1, 1);
	      
	      //*******************------------------------**********
	     //creating thread
	     fprintf(stderr, "Worker %s was initialized \n", arg1);
	     pthread_create(&thr, NULL, do_job, worker);
	     worker->other = NULL;
	     worker->thr = thr;	     
         if (worker_head == NULL) {
             worker_head = worker;
         } else {
             worker_st *crs = worker_head;
             while (crs->other != NULL) {
                 crs = crs->other;
             }
             crs->other = worker;
         }
	     

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
	    fprintf(stderr, "%s was added\n", arg1);
	     comp->other = NULL;     
         if (component_head == NULL) {
             component_head = comp;
         } else {
             component_st *crs = component_head;
             while (crs->other != NULL) {
                 crs = crs->other;
             }
             crs->other = comp;
         }
        //Comp created
	     
	     //-------WAKING UP WORKERS
        /*worker_st *crs = worker_head;

        while (crs != NULL) {
            if (crs->pl == comp->now_pl) {
                sem_post(&crs->wakes);
            }
            crs = crs->other;
        }*/
	    
	} else if (strcmp(cmd, "end") == 0 && arg1 && !arg2) {
	    /* tell the worker to finish
	     * the worker has to finish their work first
	     * you should not wait here for the worker to finish */
	    /* if the worker is waiting for work
	       you need to wakeup the worker */ 
	        worker_st *crs = worker_head;
	        worker_st *prv = crs;
	        
	        while (crs != NULL){
	            if (!strcmp(arg1, crs->work_n)){
	                crs->ends = 1;
	                //sem_post(&crs->wakes);
	                pthread_join(crs->thr, NULL);
	                
	                if (crs == worker_head){
	                    worker_head = crs->other;
	                } else{
	                    prv->other = crs->other;
	                }
	                crs->other = NULL;
	                fprintf(stderr, "End of worker %s\n", crs->work_n);
	                free(crs->work_n);
	                free(crs);
	                break;
	            }
	            prv = crs;
	            crs = crs->other;
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
        work_pl->other = NULL;
	    pthread_mutex_init(&(work_pl->mutex), NULL);
	    fprintf(stderr, "%s was added\n", place_str[work_pl->pl]);
        if (workplace_head == NULL) {
            workplace_head = work_pl;
        } else {
            workplace *tail = workplace_head;
            while (tail != NULL) {
                if (tail->other != NULL) {
                    tail = tail->other;
                } else {
                    tail->other = work_pl;
                    break;
                }
            }
        }

	    
	    
	} else if (strcmp(cmd, "remove") == 0 && arg1 && !arg2) {
	    /* if you cannot remove empty place you cannot wait for finish work */
            workplace *crs = workplace_head;
            workplace *prv = crs;
            
            while (crs != NULL){
                
                if (!strcmp(place_str[crs->pl],arg1) && crs->works == 0){
                    pthread_mutex_lock(&crs->mutex);
                    pthread_mutex_lock(&main_m);
                    if (crs == workplace_head){
                        workplace_head = crs->other;
                    } else {
                        prv->other = crs->other;
                    }
                    crs->other = NULL;
                    crs->ends = 1;
                    fprintf(stderr, "Removing %s\n", place_str[crs->pl]);
                    pthread_mutex_unlock(&main_m);
                    pthread_mutex_unlock(&crs->mutex);
                    pthread_mutex_destroy(&crs->mutex);
                    free(crs);
                    
                    break;
                }
                
                prv = crs;
                crs = crs->other;
            }
            pthread_mutex_destroy(&main_m); 
	} else {
	    fprintf(stderr, "Invalid command: %s\n", line);
	}
	free(line);
    }
    scanned_EOF=1;
    
    
    //TODO- stop this thread and let workers do their job
    //fprintf(stderr,"zde\n");
    while (is_there_work() == 1){
        //fprintf(stderr,"There is still something to do\n");
        wake_them();
        //sem_wait(&main_wake);
    }
    
    //free worker
    fprintf(stderr, "freeing all workers\n");
    worker_st *crs1 = worker_head;
    while (crs1 != NULL){
        worker_st *will_free = crs1;
        crs1 = crs1->other;
        will_free->ends = 1;
        sem_post(&will_free->wakes);
        pthread_join(will_free->thr, NULL);
        
        if (will_free == worker_head){
            worker_head = crs1;
        }
        free(will_free->work_n);
        free(will_free);
        fprintf(stderr, "Worker was freed\n");

    }
    //free workplaces
    workplace *crs = workplace_head;
    fprintf(stderr, "freeing workplaces\n");
    while (crs != NULL){
            workplace *free_ = crs;
            crs = free_->other;
            
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
        curs = curs->other;
        if (will_fr == component_head){
            component_head = curs;
        }
        free(will_fr);
        fprintf(stderr, "Comp was freed\n");
     }
     
    return 0;
 
    /* Wait for every worker to finish their work. Nobody should be
     * able to continue. */
}


