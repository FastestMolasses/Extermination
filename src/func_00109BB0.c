// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.96): clears two fields of the struct pointed at by a0->0x40.
struct func_00109BB0_s { char pad[0x40]; long *p; };

void func_00109BB0(struct func_00109BB0_s *a) {
    long *q = a->p;
    *(int *)((char *)q + 0x70) = 0;
    *(long *)((char *)q + 0x78) = 0;
}
