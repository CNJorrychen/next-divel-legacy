#include "NextKernel_GDT.h"
#include "ND_Bus.h"
#include "NextKernel_ISR.h"
#include "ND_Memory.h"
// descriptor_tables.c - Initialises the GDT and IDT, and defines the 
//                       default ISR and IRQ handler.
//                       Based on code from Bran's kernel development tutorials.
//                       Rewritten for JamesM's kernel development tutorials.
//

// Lets us access our ASM functions from our C code.
extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

// Internal function prototypes.
static void NextKernel_GDT_Start();
static void NextKernel_IDT_Start();
static void NextKernel_GDT_SetGate(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);
static void NextKernel_IDT_SetGate(uint8_t,uint32_t,uint16_t,uint8_t);

NextKernel_GDT_Entry gdt_entries[5];
NextKernel_GDT_Ptr   gdt_ptr;
NextKernel_IDT_Entry idt_entries[256];
NextKernel_IDT_Ptr   idt_ptr;

// Extern the ISR handler array so we can nullify them on startup.
extern isr_t interrupt_handlers[];

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void NextKernel_GDT_Init()
{

    // Initialise the global descriptor table.
    NextKernel_GDT_Start();
    // Initialise the interrupt descriptor table.
    NextKernel_IDT_Start();
    // Nullify all the interrupt handlers.
    ND_Memory_Memset((uint8_t *)&interrupt_handlers, 0, sizeof(isr_t)*256);
}

static void NextKernel_GDT_Start()
{
    gdt_ptr.limit = (sizeof(NextKernel_GDT_Entry) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    NextKernel_GDT_SetGate(0, 0, 0, 0, 0);                // Null segment
    NextKernel_GDT_SetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    NextKernel_GDT_SetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    NextKernel_GDT_SetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    NextKernel_GDT_SetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    gdt_flush((uint32_t)&gdt_ptr);
}

// Set the value of one GDT entry.
static void NextKernel_GDT_SetGate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

static void NextKernel_IDT_Start()
{
    idt_ptr.limit = sizeof(NextKernel_IDT_Entry) * 256 -1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    ND_Memory_Memset((uint8_t *)&idt_entries, 0, sizeof(NextKernel_IDT_Entry)*256);

    // Remap the irq table.
    ND_Bus_outb(0x20, 0x11);
    ND_Bus_outb(0xA0, 0x11);
    ND_Bus_outb(0x21, 0x20);
    ND_Bus_outb(0xA1, 0x28);
    ND_Bus_outb(0x21, 0x04);
    ND_Bus_outb(0xA1, 0x02);
    ND_Bus_outb(0x21, 0x01);
    ND_Bus_outb(0xA1, 0x01);
    ND_Bus_outb(0x21, 0x0);
    ND_Bus_outb(0xA1, 0x0);

    NextKernel_IDT_SetGate( 0, (uint32_t)isr0 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 1, (uint32_t)isr1 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 2, (uint32_t)isr2 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 3, (uint32_t)isr3 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 4, (uint32_t)isr4 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 5, (uint32_t)isr5 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 6, (uint32_t)isr6 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 7, (uint32_t)isr7 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 8, (uint32_t)isr8 , 0x08, 0x8E);
    NextKernel_IDT_SetGate( 9, (uint32_t)isr9 , 0x08, 0x8E);
    NextKernel_IDT_SetGate(10, (uint32_t)isr10, 0x08, 0x8E);
    NextKernel_IDT_SetGate(11, (uint32_t)isr11, 0x08, 0x8E);
    NextKernel_IDT_SetGate(12, (uint32_t)isr12, 0x08, 0x8E);
    NextKernel_IDT_SetGate(13, (uint32_t)isr13, 0x08, 0x8E);
    NextKernel_IDT_SetGate(14, (uint32_t)isr14, 0x08, 0x8E);
    NextKernel_IDT_SetGate(15, (uint32_t)isr15, 0x08, 0x8E);
    NextKernel_IDT_SetGate(16, (uint32_t)isr16, 0x08, 0x8E);
    NextKernel_IDT_SetGate(17, (uint32_t)isr17, 0x08, 0x8E);
    NextKernel_IDT_SetGate(18, (uint32_t)isr18, 0x08, 0x8E);
    NextKernel_IDT_SetGate(19, (uint32_t)isr19, 0x08, 0x8E);
    NextKernel_IDT_SetGate(20, (uint32_t)isr20, 0x08, 0x8E);
    NextKernel_IDT_SetGate(21, (uint32_t)isr21, 0x08, 0x8E);
    NextKernel_IDT_SetGate(22, (uint32_t)isr22, 0x08, 0x8E);
    NextKernel_IDT_SetGate(23, (uint32_t)isr23, 0x08, 0x8E);
    NextKernel_IDT_SetGate(24, (uint32_t)isr24, 0x08, 0x8E);
    NextKernel_IDT_SetGate(25, (uint32_t)isr25, 0x08, 0x8E);
    NextKernel_IDT_SetGate(26, (uint32_t)isr26, 0x08, 0x8E);
    NextKernel_IDT_SetGate(27, (uint32_t)isr27, 0x08, 0x8E);
    NextKernel_IDT_SetGate(28, (uint32_t)isr28, 0x08, 0x8E);
    NextKernel_IDT_SetGate(29, (uint32_t)isr29, 0x08, 0x8E);
    NextKernel_IDT_SetGate(30, (uint32_t)isr30, 0x08, 0x8E);
    NextKernel_IDT_SetGate(31, (uint32_t)isr31, 0x08, 0x8E);
    NextKernel_IDT_SetGate(32, (uint32_t)irq0, 0x08, 0x8E);
    NextKernel_IDT_SetGate(33, (uint32_t)irq1, 0x08, 0x8E);
    NextKernel_IDT_SetGate(34, (uint32_t)irq2, 0x08, 0x8E);
    NextKernel_IDT_SetGate(35, (uint32_t)irq3, 0x08, 0x8E);
    NextKernel_IDT_SetGate(36, (uint32_t)irq4, 0x08, 0x8E);
    NextKernel_IDT_SetGate(37, (uint32_t)irq5, 0x08, 0x8E);
    NextKernel_IDT_SetGate(38, (uint32_t)irq6, 0x08, 0x8E);
    NextKernel_IDT_SetGate(39, (uint32_t)irq7, 0x08, 0x8E);
    NextKernel_IDT_SetGate(40, (uint32_t)irq8, 0x08, 0x8E);
    NextKernel_IDT_SetGate(41, (uint32_t)irq9, 0x08, 0x8E);
    NextKernel_IDT_SetGate(42, (uint32_t)irq10, 0x08, 0x8E);
    NextKernel_IDT_SetGate(43, (uint32_t)irq11, 0x08, 0x8E);
    NextKernel_IDT_SetGate(44, (uint32_t)irq12, 0x08, 0x8E);
    NextKernel_IDT_SetGate(45, (uint32_t)irq13, 0x08, 0x8E);
    NextKernel_IDT_SetGate(46, (uint32_t)irq14, 0x08, 0x8E);
    NextKernel_IDT_SetGate(47, (uint32_t)irq15, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr);
}

static void NextKernel_IDT_SetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}
