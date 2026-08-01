// COMPILER: eegcc
// CFLAGS: -O2
//
// newlib-style `std()`: initialize a stdio stream descriptor in place.
// Zeroes _p/_r/_w (0x00/0x04/0x08), _bf._base (0x10) and _lbfsize (0x18);
// stores the flag word and fd (0x0C/0x0E); points _cookie (0x1C) at the
// stream itself; installs the four I/O vector entries (0x20 read, 0x24
// write, 0x28 seek, 0x2C close) that func_0011FD88 later dispatches
// through; and parks the reent/data pointer at 0x54.
//
// Statement order is load-bearing, not cosmetic. ee-gcc 2.9's pre-reload
// list scheduler ranks equal-priority insns by INSN_REG_WEIGHT (number of
// pseudos that die in the insn), lower first. Keeping the +0x54 store LAST
// in the source makes both `c` and `p` die there (weight -2), so it is
// issued first; it simultaneously stops `p` from dying at the +0x1C
// self-pointer store (weight 0), which lets that store sink past the five
// zero stores into the `jr $ra` delay slot, exactly as the target does.
// Writing the stores in target-emission order instead yields 99.27%.

extern void vtable_a0_at_0011FE90_off20(void);
extern void vtable_a0_at_0011FE90_off24(void);
extern void vtable_a0_at_0011FE90_off28(void);
extern void vtable_a0_at_0011FE90_off2C(void);

void init_vtable_a0_at_0011FE90(void *obj, short a, short b, int c)
{
    unsigned char *p = (unsigned char *)obj;

    *(int *)(p + 0x0) = 0;
    *(int *)(p + 0x4) = 0;
    *(int *)(p + 0x8) = 0;
    *(short *)(p + 0xC) = a;
    *(short *)(p + 0xE) = b;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x18) = 0;
    *(void **)(p + 0x1C) = p;
    *(void **)(p + 0x20) = (void *)vtable_a0_at_0011FE90_off20;
    *(void **)(p + 0x24) = (void *)vtable_a0_at_0011FE90_off24;
    *(void **)(p + 0x28) = (void *)vtable_a0_at_0011FE90_off28;
    *(void **)(p + 0x2C) = (void *)vtable_a0_at_0011FE90_off2C;
    *(int *)(p + 0x54) = c;
}
