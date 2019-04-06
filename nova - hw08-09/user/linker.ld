OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_start)

PHDRS
{
	data    PT_LOAD FLAGS(6);
	text    PT_LOAD FLAGS(5);
}

SECTIONS
{
    . =  0x2000;
    stack_top = .;
    . += SIZEOF_HEADERS;

    .data  :
    {
        BYTE(0x42);
        *(.rodata*);
        *(.data*);
        *(.bss*);
        . = ALIGN(0x1000);
    } : data

    .text :
    {
        *(.text*);
    } : text

    .init_array :
    {
        PROVIDE (CTORS_L = .);
        KEEP (*(.init_array.65534 .ctors.00001))
        PROVIDE (CTORS_G = .);
        KEEP (*(.init_array.* .ctors.*))
        KEEP (*(.init_array .ctors))
        PROVIDE (CTORS_E = .);
    } : text

}
