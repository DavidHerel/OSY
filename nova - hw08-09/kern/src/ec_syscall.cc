#include "bits.h"
#include "ec.h"
#include "assert.h"
#include "cpu.h"
#include "ptab.h"
#include "multiboot.h"
#include "elf.h"
#include "memory.h"
#include "extern.h"
#include "kalloc.h"
#include "kalloc.h"
#include "memory.h"
#include "stdio.h"
#include "extern.h"
#include "string.h"


typedef enum {
    sys_print      = 1,
    sys_sum        = 2,
    sys_break      = 3,
    sys_thr_create = 4,
    sys_thr_yeild  = 5,
} Syscall_numbers;

void Ec::syscall_handler (uint8 a)
{
    // Get access to registers stored during entering the system - see
    // entry_sysenter in entry.S
    Sys_regs * r = current->sys_regs();
    Syscall_numbers number = static_cast<Syscall_numbers> (a);

    switch (number) {
        case sys_print: {
            char *data = reinterpret_cast<char*>(r->esi);
            unsigned len = r->edi;
            for (unsigned i = 0; i < len; i++)
                printf("%c", data[i]);
            break;
        }
        case sys_sum: {
            // Naprosto nepotřebné systémové volání na sečtení dvou čísel
            int first_number = r->esi;
            int second_number = r->edi;
            r->eax = first_number + second_number;
            break;
        }
        case sys_break: {
            
            //if mensi nez mez
            if (r->esi < Ec::break_min && r->esi != 0){
                r->eax = 0;
                break;
            }
            
            //if 0 posun
            if (r->esi == 0 || r->esi == Ec::break_current){
                r->eax = Ec::break_current; 
                break;
            }
            
            //if presahnuti mezi
            if (r->esi >= 0xC0000000){
                r->eax = 0;
                break;
            }
            r->eax = Ec::break_current;
            if (r->esi != 0){
         //   printf("r-esi %lx \n", r->esi);
          //  printf("break curr %lx \n", Ec::break_current);
            //if r->esi is bigger than current break -> alloc it to e->esi and curr = esi
            //pages which are alocated i will set to 0
                if (r->esi > Ec::break_min && r->esi > align_up(Ec::break_current, PAGE_SIZE)){
             //       printf("Goes up -> increase\n");
                    //ze by chyba v align up?
                    //oukej chuba v tom neni >D
                    
                    mword virt = align_up(Ec::break_current, PAGE_SIZE); 
               //     printf("virt %lx \n", virt);
                    void * p = reinterpret_cast<void *>(Ec::break_current);
                        if (Ec::break_current != virt){
                            memset(p, 0, virt - Ec::break_current);
                        }
                //        printf("virt %lx \n", virt);
                   //mword virt = ((Ec::break_current -1 + PAGE_SIZE) & 0xfffff000);
                    while (virt < (r->esi)){
                        bool mappingFail;
                        void *ptr = Kalloc::allocator.alloc_page(1, Kalloc::FILL_0);
                        //if there is no error -> alloc page
                                                    //namapuji
                            mappingFail = Ptab::insert_mapping (virt,Kalloc::virt2phys (ptr) , Ptab::PRESENT | Ptab::RW | Ptab::USER);                      
                        if (ptr != NULL && mappingFail){

                            
                            virt += PAGE_SIZE;
                            
                        //if there is error-> delete all allocated pages
                        } else if (ptr == NULL || !mappingFail){
                //            printf("Deleting all pages until break current\n");
                //            printf("virt %lx \n", virt);
                            virt -= PAGE_SIZE;
                            while(virt >= (Ec::break_current)){
                                void *ptr2 = Kalloc::phys2virt((Ptab::get_mapping(virt)) & 0xfffff000);
                                Kalloc::allocator.free_page(ptr2);
                                Ptab::insert_mapping(virt, 0, 0);
                                virt -= PAGE_SIZE;
                                 
                            }
                            r->eax = 0;
                            break;
                        }
                    }
                    
                    if (r->eax != 0){
                         Ec::break_current = r->esi;
                    }
               //     printf("reax = %lx\n", r->eax);
                //    printf("breakc = %lx\n", Ec::break_current);
                    break;
                }
                
                
                //deleting pages if esi is smaller than current
                else if (r->esi >= Ec::break_min && r->esi < Ec::break_current){
               //     printf("Goes down -> decrease\n");
                    //temp promena -> proto abych sel po strankach
                    //ze by chyba v allign up?
                    //okej chyba v align up neni xD
                    mword virt = align_dn(Ec::break_current, PAGE_SIZE); 
                   // mword virt = ((Ec::break_current -1 - PAGE_SIZE) & 0xfffff000);
              //     printf("reaxd = %lx\n", r->eax);
              //      printf("virtd = %lx\n", align_up(r->esi, PAGE_SIZE));
                    while (virt > align_up(r->esi, PAGE_SIZE)){
                        virt -= PAGE_SIZE;
                        //jdu po jedne strance dolu
                        void *ptr = Kalloc::phys2virt((Ptab::get_mapping(virt)) & 0xfffff000);
                                    //if mensi nez mez
                        if (ptr != NULL){
                            //zde chyba kernel: attempt to free invalid page

                            Kalloc::allocator.free_page(ptr);
                            Ptab::insert_mapping(virt, 0, 0);
                        } else{
                            break;
                        }

                        
                    }
                    if (r->eax != 0){
                        Ec::break_current = r->esi; 
                    }
              //      printf("reax = %lx\n", r->eax);
              //      printf("breakc = %lx\n", Ec::break_current);
                    break;
                }
                if (r->eax != 0){
                    Ec::break_current = r->esi; 
                }
                break;
            }
            break;
        }
        default:
            printf ("unknown syscall %d\n", number);
            break;
    };

    ret_user_sysexit();
}
