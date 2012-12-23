#ifndef REGISTERS_HEADER_FILE
#define REGISTERS_HEADER_FILE

/* These registers must be registers that are preserved over function calls
   in C.  What I mean by this is that if we're using these registers and we
   call a C function, then when that C function returns these registers are
   still what they were before we made the call. */

#ifdef PROCESSOR_ARM
     register uint32 reg68k_pc asm ("r7");
     register uint32 *reg68k_regs asm ("r8");
     register t_sr reg68k_sr asm ("r9");
#endif

#ifdef PROCESSOR_SPARC
     register uint32 reg68k_pc asm ("5");
     register uint32 *reg68k_regs asm ("6");
     register t_sr reg68k_sr asm ("7");
#endif

#ifdef PROCESSOR_i386
     register uint32 reg68k_pc asm ("%ebx");
     register uint32 *reg68k_regs asm ("%edi");
     register t_sr reg68k_sr asm ("%esi");
#endif

#ifdef PROCESSOR_x86_64
     register uint32 reg68k_pc asm ("%rbx");
     register uint32 *reg68k_regs asm ("%r14");
     register t_sr reg68k_sr asm ("%r15");
#endif

#ifdef PROCESSOR_MIPS
     register uint32 reg68k_pc asm ("%s0");
     register uint32 *reg68k_regs asm ("%s1");
     register t_sr reg68k_sr asm ("%s2");
#endif

#if (!(defined(PROCESSOR_ARM) || defined(PROCESSOR_SPARC) \
    || defined(PROCESSOR_i386) || defined(PROCESSOR_x86_64) \
    || defined(PROCESSOR_MIPS)))
     extern uint32 reg68k_pc;
     extern uint32 *reg68k_regs;
     extern t_sr reg68k_sr;
#endif

#endif /* REGISTERS_HEADER_FILE */
