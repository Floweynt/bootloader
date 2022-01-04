#include "realmode.h"
// this shall enter 16 bit mode
// execute interrupt
// and return to 32 bit mode

#define SWITCH_INTERRUPT_N(v, in, out)  \
    case v:                             \
    __asm__ __volatile__(               \
        "int $" #v                     \
        : "=a"(out.a),                  \
          "=b"(out.b),                  \
          "=c"(out.c),                  \
          "=d"(out.d),                  \
          "=S"(out.S),                  \
          "=D"(out.D)                   \
        : "a"(in.a),                    \
          "b"(in.b),                    \
          "c"(in.c),                    \
          "d"(in.d),                    \
          "S"(in.S),                    \
          "D"(in.D)                     \
    );                                  \
    __asm__ __volatile__(               \
        "pushf\n"                       \
        "pop %%eax"                      \
        : "=a"(out.flags)               \
        :                               \
    );                                  \
    break;


uint16_t realmode_sp = 0x3000;
uint16_t pmode_sp = 0x7000;

cpustate bios_interrupt_pmode(uint8_t n, cpustate state)
{
    BEGIN_32;
    static uint8_t gdt[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Null entry
        0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0x8f, 0x00, // code
        0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0x8f, 0x00, // data
    };

    struct {
        uint16_t limit = sizeof(gdt);
        uint32_t base = (uint32_t)gdt;
    } __attribute__((packed)) gdtr;

    struct {
        uint16_t limit = sizeof(gdt);
        uint32_t base = (uint32_t)gdt;
    } __attribute__((packed)) idtr;

    __asm__ goto(
        "cli\n"
        "lgdt %0\n"
        "ljmp $0x8, $%l1"
        : 
        : "m"(gdtr)
        :
        : farjump_16
    );
    BEGIN_16;
farjump_16:
    __asm__ __volatile__(
        "mov $0x10, %%ax\n"
    	"mov %%ax, %%ds\n"
    	"mov %%ax, %%es\n"
    	"mov %%ax, %%fs\n"
    	"mov %%ax, %%gs\n"
    	"mov %%ax, %%ss\n"
        :
        :
        : "ax"
    );

    __asm__ __volatile__("lidt %0" : : "m"(idtr));
    __asm__ goto(
        "mov %%cr0, %%eax\n"
        "and $0xfe, %%al\n"
        "mov %%eax, %%cr0\n"
        "ljmp $0x0, $%l0"
        :
        :
        : "eax"
        : realmode
    );
realmode:
    __asm__ __volatile__(
        "xor %%ax, %%ax\n"
    	"mov %%ax, %%ds\n"
    	"mov %%ax, %%es\n"
    	"mov %%ax, %%fs\n"
    	"mov %%ax, %%gs\n"
    	"mov %%ax, %%ss\n"
        "mov %%sp, pmode_sp\n"
        "mov realmode_sp, %%sp"
        :
        :
        : "ax"
    );


    // execute interrupt and save
    auto ret = bios_interrupt_realmode(n, state);
    protected_mode();
    BEGIN_32;
    return ret;
}

cpustate bios_interrupt_realmode(uint8_t n, cpustate in)
{
    BEGIN_16;
    cpustate out;
    switch(n) 
    {
        SWITCH_INTERRUPT_N(0x5, in, out)
        SWITCH_INTERRUPT_N(0x8, in, out)
        SWITCH_INTERRUPT_N(0x9, in, out)
        SWITCH_INTERRUPT_N(0x10, in, out)
        SWITCH_INTERRUPT_N(0x11, in, out)
        SWITCH_INTERRUPT_N(0x12, in, out)
        SWITCH_INTERRUPT_N(0x13, in, out)
        SWITCH_INTERRUPT_N(0x14, in, out)
        SWITCH_INTERRUPT_N(0x15, in, out)
        SWITCH_INTERRUPT_N(0x16, in, out)
        SWITCH_INTERRUPT_N(0x17, in, out)
    }

    return out;
    BEGIN_32;
}

void protected_mode()
{
    BEGIN_16;

    static uint8_t gdt[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Null entry
        0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xCF, 0x00, // code
        0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00, // data
    };

    struct {
        uint16_t limit = sizeof(gdt);
        uint32_t base = (uint32_t)gdt;
    } __attribute__((packed)) gdtr;

    // disable interrupts
    __asm__ __volatile__("cli");
    // enable protection
    __asm__ __volatile__(
        "mov %cr0, %eax\n"
        "or $1, %al\n"
        "mov %eax, %cr0"
    );
    // load gdt
    __asm__ __volatile__("lgdt %0" : : "m"(gdtr));
    __asm__ goto (
        "ljmp $0x8, $%l0\n"
        :
        :
        :
        : pmode
    );
pmode:
    BEGIN_32;
    __asm__ __volatile__(
        "mov $0x10, %%ax\n"
    	"mov %%ax, %%ds\n"
    	"mov %%ax, %%es\n"
    	"mov %%ax, %%fs\n"
    	"mov %%ax, %%gs\n"
    	"mov %%ax, %%ss\n"
        "mov %%sp, realmode_sp\n"
        "mov pmode_sp, %%sp"
        :
        :
        : "ax"
    );
}
