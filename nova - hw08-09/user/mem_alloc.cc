#include <stdio.h>

unsigned my_syscall2(unsigned w0, unsigned w1)
{
    asm volatile (
        "   mov %%esp, %%ecx    ;"
        "   mov $1f, %%edx      ;"
        "   sysenter            ;"
        "1:                     ;"
        : "+a" (w0) : "S" (w1) : "ecx", "edx", "memory");
    return w0;
}

void *my_brk(void *address)
{
    return (void*)my_syscall2(3, (unsigned)address);
}


 typedef struct list {
    struct list *next;
    unsigned int size;
    unsigned is_free;
} list_t;


list_t *start, *end;


extern "C" int my_free(void *block){
    if (start == NULL){
        return 1;
    }
    if (block == NULL){
        return 1;
    }
    if(!block){
        return 1;
    }
    
    //we need pointer on a block we want to free
    list_t *header = (list_t*)block - 1;
    if(header->is_free){
        return 1;
    }
    
    list_t *temp;
    //we first find the end of the current block
    if ((char*)block + header->size == my_brk(0)){
        if (start == end){
            start = end = NULL;
        } else {
            temp = start;
            while (temp){
                if (temp->next == end){
                    temp->next = NULL;
                    end = temp;
                }
                temp = temp->next;
            }
        }
        
        //call brk to release actual block
        my_brk((void*)((unsigned int)(my_brk(0)) - sizeof(list_t) - header->size));
        return 0;
    }
    //in the case the block is not the last one in the linked list
    //we set is_free of its header
    header->is_free = 1;
    return 0;
}

/**
* Function on finding block for allocation - using first fit approach
*/
list_t *retBlock(unsigned int size){

    list_t *now = start;
    while (now){
        if (now->is_free && now->size >= size){
            return now;
        }
        now = now->next;
    }
    return NULL;
}

extern "C" void *my_malloc(unsigned int size){ 
    //if requested size is zero -> return null
    if (!size){
        return NULL;
    }
    
    //for a valid size -> first fit approach in linked list
    //(vezmi prvni blok od zacatku haldy, ktery ma alespon pozadovanou velikost)
    list_t *header = retBlock(size);
    
    //if we find block -> mark him as not free
    // and return a pointer to that block
    if (header){
        header->is_free = 0;
        
        //we need second byte -> this is why + 1
        return (void*)(header +1);
    }
    
    //if we didnt find the block
    //-> we need to extend the heap by calling brk()
    //heap needs to be extended by size that fits the requested size as well as header
    //compute whole size
    unsigned int whole_size = sizeof(list_t) + size;
    
    
    //request OS to increment the program break
    void *block = my_brk((void*)((unsigned int)my_brk(0) + whole_size));
    
    //if there were mistake return null
    if (block == (void*) -1){
        return 0;
    }
    
    //make space for header
    header = (list_t*)block;
    
    //fill the header with requested size
    header->size = size;
    
    //mark it as not free
    header->is_free = 0;
    
    //update next pointer
    header->next = NULL;
    
    //update head and end
    if (!start){
        start = header;
    }
    if (end){
        end->next = header;
    }
    end = header;
    return (void*)(header +1);
}

