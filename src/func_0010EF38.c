// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK free-slot allocator (vram 0x0010EF38): under an interrupt-disabled
// section, scan the 16-entry table at D_00279110 for the first entry whose
// +0x4 word is 0, claim it with 0x10000000, and return it (0 if the table is
// full).
//
// Matching note (ee-gcc 2.9-991111-01, -O2): `di`/`sync.p`/`mfc0` must be
// three separate asm statements. The MIPS backend pads the branch that
// consumes an asm result with nops and decrements the pad count once per
// emitted insn, so folding them into one or two statements adds a spurious
// nop before `bnez`.

typedef struct Ent {
    unsigned int f0;
    unsigned int f4;
    unsigned int f8;
    unsigned int fC;
} Ent;

extern Ent D_00279110[16];

Ent *func_0010EF38(void)
{
    Ent *p;
    unsigned int st;

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r" (st));
    } while (st & 0x10000);

    for (p = D_00279110; p < &D_00279110[16]; p++) {
        if (p->f4 == 0) {
            p->f4 = 0x10000000;
            __asm__ __volatile__("ei");
            return p;
        }
    }
    __asm__ __volatile__("ei");
    return 0;
}
