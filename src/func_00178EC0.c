// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-channel motion-impulse helper, dispatched on the actor's state word at
// 0x24C. For state 2 (negate) and state 3 (forward) it computes a scaled axis
// value = pos.x(0x38) * (+/-)tilt_table[type@0x23F] into a 4-float scratch vector
// at 0x700038A0 (only .x set, .y/.z/.w zeroed), runs func_001026A0 to transform
// it through the actor's 0xD0 matrix into 0x700038B0, then accumulates the
// transformed x/z into the actor's velocity fields 0xB0 / 0xB8. The scratch
// vector lives at a fixed scratchpad-region address (0x7000xxxx): the direct
// loads/stores use absolute lui-0x7000 addressing (recomputed per access), while
// the two pointers handed to func_001026A0 are taken as named-symbol relocs
// (&D_700038B0 / &D_700038A0) -- both reproduced exactly here.
extern void func_001026A0(float *, void *, float *);
extern float D_00248730[];
extern float D_700038A0;
extern float D_700038B0;

void func_00178EC0(char *arg0) {
    int mode;

    mode = *(int *)(arg0 + 0x24C);
    if (mode == 2) {
        *(volatile float *)0x700038A0 = *(float *)(arg0 + 0x38) * -D_00248730[*(unsigned char *)(arg0 + 0x23F)];
        *(volatile int *)0x700038A4 = 0;
        *(volatile int *)0x700038A8 = 0;
        *(volatile int *)0x700038AC = 0;
        func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(volatile float *)0x700038B0;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(volatile float *)0x700038B8;
    } else if (mode == 3) {
        *(volatile float *)0x700038A0 = *(float *)(arg0 + 0x38) * D_00248730[*(unsigned char *)(arg0 + 0x23F)];
        *(volatile int *)0x700038A4 = 0;
        *(volatile int *)0x700038A8 = 0;
        *(volatile int *)0x700038AC = 0;
        func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(volatile float *)0x700038B0;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(volatile float *)0x700038B8;
    }
}
