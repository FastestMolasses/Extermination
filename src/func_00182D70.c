// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Leaf one-time init/reset of an actor struct at p. Guard byte at 0x70003B8F
// is set on first call. Clears counters/flags, sets state byte=1, propagates
// state=2 to a linked object at p->0x1C, copies p->0x20C into p->0x1F2,
// stores float 1.0 (0x3F800000) at p->0x1F4, and -1 at p->0x24C.
// mwcc 2.3.3 reproduces the clean-store beqz delay-slot nop that 991202 fills (87.1%).
void func_00182D70(unsigned char *p) {
    unsigned char *q;
    if (*(unsigned char *)0x70003B8F == 0) {
        *(unsigned char *)0x70003B8F = 1;
    }
    *(int *)(p + 0x224) = 0;
    *(int *)(p + 0x22C) = 0;
    *(unsigned char *)(p + 0xF) = 0;
    *(short *)(p + 0x20E) = 0;
    *(unsigned char *)(p + 0) = 1;
    q = *(unsigned char **)(p + 0x1C);
    if (q != 0) {
        *(unsigned char *)(q + 4) = 2;
    }
    *(short *)(p + 0x1F2) = *(short *)(p + 0x20C);
    *(int *)(p + 0x1F8) = 0;
    *(unsigned char *)(p + 0x2F3) = 0;
    *(int *)(p + 0x1F4) = 0x3F800000;
    *(unsigned char *)(p + 0x23F) = 0;
    *(int *)(p + 0x38) = 0;
    *(int *)(p + 0x240) = 0;
    *(int *)(p + 0x24C) = -1;
    *(short *)(p + 0x276) = 0;
}
