#include <stdio.h>
#include "mem_alloc.cc"

#define ORIGINAL_BREAK_ADDRESS 0x4000 
#define MIN_ADDRESS 0x1000 
#define MAX_ADDRESS 0xC0000000 
#define SOME_ADDRESS (void*)(10*0x1000) 
#define BIG_ADDRESS (void*)(0x10000000) 
#define PAGE_BITS       12 
#define PAGE_SIZE       (1 << PAGE_BITS) 
#define PAGE_MASK       (PAGE_SIZE - 1)

unsigned syscall2(unsigned w0, unsigned w1)
{
    asm volatile (
        "   mov %%esp, %%ecx    ;"
        "   mov $1f, %%edx      ;"
        "   sysenter            ;"
        "1:                     ;"
        : "+a" (w0) : "S" (w1) : "ecx", "edx", "memory");
    return w0;
}

void *brk(void *address)
{
    return (void*)syscall2(3, (unsigned)address);
}

int main ()
{
    printf("Hello world !\n");
    
    
    void* ahoj1 = my_malloc(16);
	printf("address from malloc is: %p\n", ahoj1);
	void* ahoj2 = my_malloc(16);
	void* ahoj3 = my_malloc(16);

	printf("\nFREEEEEEE INCOMMING!\n\n"); 

	my_free(ahoj1);
	my_free(ahoj2);
	my_free(ahoj3);
	
	ahoj1 = my_malloc(16); 
	printf("address from malloc is: %p\n", ahoj1);
	
	ahoj2 = my_malloc(16);
	printf("address from malloc is: %p\n", ahoj2);

	ahoj3 = my_malloc(16);
	printf("address from malloc is: %p\n", ahoj3);

	void* ahoj5 = my_malloc(16);
	printf("address from malloc is: %p\n", ahoj5);
	my_free(ahoj3);
    
    void *addr0 = my_malloc(0x8082); for (unsigned i = 0; i < 0x8082; i++) reinterpret_cast<char*>(addr0)[i] = 0x1; void *addr1 = my_malloc(0x442); for (unsigned i = 0; i < 0x442; i++) reinterpret_cast<char*>(addr1)[i] = 0x2; void *addr2 = my_malloc(0x9bcd); for (unsigned i = 0; i < 0x9bcd; i++) reinterpret_cast<char*>(addr2)[i] = 0x3; my_free(addr1); addr1 = addr2; addr2 = my_malloc(0xf996); for (unsigned i = 0; i < 0xf996; i++) reinterpret_cast<char*>(addr2)[i] = 0x4; void *addr3 = my_malloc(0xb8de); for (unsigned i = 0; i < 0xb8de; i++) reinterpret_cast<char*>(addr3)[i] = 0x5; my_free(addr3); addr3 = my_malloc(0x550); for (unsigned i = 0; i < 0x550; i++) reinterpret_cast<char*>(addr3)[i] = 0x6; void *addr4 = my_malloc(0xe861); for (unsigned i = 0; i < 0xe861; i++) reinterpret_cast<char*>(addr4)[i] = 0x7; my_free(addr2); addr2 = addr4; my_free(addr1); addr1 = addr3; addr3 = my_malloc(0xb6a1); for (unsigned i = 0; i < 0xb6a1; i++) reinterpret_cast<char*>(addr3)[i] = 0x8; addr4 = my_malloc(0xeb75); for (unsigned i = 0; i < 0xeb75; i++) reinterpret_cast<char*>(addr4)[i] = 0x9; void *addr5 = my_malloc(0x67cb); for (unsigned i = 0; i < 0x67cb; i++) reinterpret_cast<char*>(addr5)[i] = 0xa; my_free(addr4); addr4 = addr5; addr5 = my_malloc(0x4cc3); for (unsigned i = 0; i < 0x4cc3; i++) reinterpret_cast<char*>(addr5)[i] = 0xb; my_free(addr3); addr3 = addr5; addr5 = my_malloc(0x33fe); for (unsigned i = 0; i < 0x33fe; i++) reinterpret_cast<char*>(addr5)[i] = 0xc; my_free(addr1); addr1 = addr5; addr5 = my_malloc(0xbedc); for (unsigned i = 0; i < 0xbedc; i++) reinterpret_cast<char*>(addr5)[i] = 0xd; void *addr6 = my_malloc(0xac18); for (unsigned i = 0; i < 0xac18; i++) reinterpret_cast<char*>(addr6)[i] = 0xe; void *addr7 = my_malloc(0xa1d8); for (unsigned i = 0; i < 0xa1d8; i++) reinterpret_cast<char*>(addr7)[i] = 0xf; void *addr8 = my_malloc(0x3627); for (unsigned i = 0; i < 0x3627; i++) reinterpret_cast<char*>(addr8)[i] = 0x10; void *addr9 = my_malloc(0x94f4); for (unsigned i = 0; i < 0x94f4; i++) reinterpret_cast<char*>(addr9)[i] = 0x11; my_free(addr9); my_free(addr0); addr0 = addr8; my_free(addr4); addr4 = addr7; addr7 = my_malloc(0xb17a); for (unsigned i = 0; i < 0xb17a; i++) reinterpret_cast<char*>(addr7)[i] = 0x12; my_free(addr2); addr2 = addr7; addr7 = my_malloc(0x23d1); for (unsigned i = 0; i < 0x23d1; i++) reinterpret_cast<char*>(addr7)[i] = 0x13; addr8 = my_malloc(0xe70f); for (unsigned i = 0; i < 0xe70f; i++) reinterpret_cast<char*>(addr8)[i] = 0x14; addr9 = my_malloc(0x2abd); for (unsigned i = 0; i < 0x2abd; i++) reinterpret_cast<char*>(addr9)[i] = 0x15; void *addr10 = my_malloc(0x1ec6); for (unsigned i = 0; i < 0x1ec6; i++) reinterpret_cast<char*>(addr10)[i] = 0x16; void *addr11 = my_malloc(0x6f3); for (unsigned i = 0; i < 0x6f3; i++) reinterpret_cast<char*>(addr11)[i] = 0x17; void *addr12 = my_malloc(0x7de9); for (unsigned i = 0; i < 0x7de9; i++) reinterpret_cast<char*>(addr12)[i] = 0x18; void *addr13 = my_malloc(0x3475); for (unsigned i = 0; i < 0x3475; i++) reinterpret_cast<char*>(addr13)[i] = 0x19; void *addr14 = my_malloc(0x8976); for (unsigned i = 0; i < 0x8976; i++) reinterpret_cast<char*>(addr14)[i] = 0x1a; void *addr15 = my_malloc(0x7cc1); for (unsigned i = 0; i < 0x7cc1; i++) reinterpret_cast<char*>(addr15)[i] = 0x1b; void *addr16 = my_malloc(0x33d4); for (unsigned i = 0; i < 0x33d4; i++) reinterpret_cast<char*>(addr16)[i] = 0x1c; void *addr17 = my_malloc(0x193a); for (unsigned i = 0; i < 0x193a; i++) reinterpret_cast<char*>(addr17)[i] = 0x1d; void *addr18 = my_malloc(0x9fe0); for (unsigned i = 0; i < 0x9fe0; i++) reinterpret_cast<char*>(addr18)[i] = 0x1e; void *addr19 = my_malloc(0x71d6); for (unsigned i = 0; i < 0x71d6; i++) reinterpret_cast<char*>(addr19)[i] = 0x1f; my_free(addr15); addr15 = addr19; addr19 = my_malloc(0x440a); for (unsigned i = 0; i < 0x440a; i++) reinterpret_cast<char*>(addr19)[i] = 0x20; void *addr20 = my_malloc(0x90b7); for (unsigned i = 0; i < 0x90b7; i++) reinterpret_cast<char*>(addr20)[i] = 0x21; void *addr21 = my_malloc(0x3df3); for (unsigned i = 0; i < 0x3df3; i++) reinterpret_cast<char*>(addr21)[i] = 0x22; my_free(addr13); addr13 = addr21; my_free(addr7); addr7 = addr20; my_free(addr0); addr0 = addr19; my_free(addr7); addr7 = addr18; my_free(addr13); addr13 = addr17; my_free(addr5); addr5 = addr16; addr16 = my_malloc(0x9bc7); for (unsigned i = 0; i < 0x9bc7; i++) reinterpret_cast<char*>(addr16)[i] = 0x23; my_free(addr11); addr11 = addr16; my_free(addr1); addr1 = addr15; my_free(addr13); addr13 = addr14; my_free(addr6); addr6 = addr13; addr13 = my_malloc(0x5086); for (unsigned i = 0; i < 0x5086; i++) reinterpret_cast<char*>(addr13)[i] = 0x24; my_free(addr11); addr11 = addr13; my_free(addr5); addr5 = addr12; addr12 = my_malloc(0xaa8b); for (unsigned i = 0; i < 0xaa8b; i++) reinterpret_cast<char*>(addr12)[i] = 0x25; addr13 = my_malloc(0x6987); for (unsigned i = 0; i < 0x6987; i++) reinterpret_cast<char*>(addr13)[i] = 0x26; my_free(addr12); addr12 = addr13; addr13 = my_malloc(0x7db4); for (unsigned i = 0; i < 0x7db4; i++) reinterpret_cast<char*>(addr13)[i] = 0x27; addr14 = my_malloc(0xc140); for (unsigned i = 0; i < 0xc140; i++) reinterpret_cast<char*>(addr14)[i] = 0x28; addr8 = addr14; my_free(addr8); addr8 = addr13; my_free(addr2); addr2 = addr12; my_free(addr4); addr4 = addr11; my_free(addr8); addr8 = addr10; my_free(addr2); addr2 = addr9; addr9 = my_malloc(0xab0c); for (unsigned i = 0; i < 0xab0c; i++) reinterpret_cast<char*>(addr9)[i] = 0x29; my_free(addr3); addr3 = addr9; my_free(addr4); addr4 = addr8; addr8 = my_malloc(0xd4a8); for (unsigned i = 0; i < 0xd4a8; i++) reinterpret_cast<char*>(addr8)[i] = 0x2a; my_free(addr2); addr2 = addr8; my_free(addr6); addr6 = addr7; addr7 = my_malloc(0x7de7); for (unsigned i = 0; i < 0x7de7; i++) reinterpret_cast<char*>(addr7)[i] = 0x2b; addr8 = my_malloc(0x7896); for (unsigned i = 0; i < 0x7896; i++) reinterpret_cast<char*>(addr8)[i] = 0x2c; my_free(addr0); addr0 = addr8; my_free(addr5); addr5 = addr7; my_free(addr2); addr2 = addr6; my_free(addr0); addr0 = addr5; addr5 = my_malloc(0x8079); for (unsigned i = 0; i < 0x8079; i++) reinterpret_cast<char*>(addr5)[i] = 0x2d; my_free(addr3); addr3 = addr5; addr5 = my_malloc(0x3a86); for (unsigned i = 0; i < 0x3a86; i++) reinterpret_cast<char*>(addr5)[i] = 0x2e; my_free(addr0); addr0 = addr5; my_free(addr0); addr0 = addr4; my_free(addr3); addr3 = my_malloc(0x72fc); for (unsigned i = 0; i < 0x72fc; i++) reinterpret_cast<char*>(addr3)[i] = 0x2f; my_free(addr1); addr1 = addr3; my_free(addr1); addr1 = addr2; my_free(addr0); addr0 = addr1; my_free(addr0);
	
    printf("\njkdsa\n");
    while (1) ;
}
