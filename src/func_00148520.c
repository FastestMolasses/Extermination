// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Three-phase reaction tick for actor `a0` with anim/behaviour context `a1`:
// phase 0 arms the reaction (resets the pose scale/flags at ctx+0x40/0x83/0x88/0x50,
// masks the effect bitfield at ctx+0x8D down to bit 0, starts anim clip 7, and if the
// pending-event flag at ctx+0x86 is set clears it, runs func_0021C040 on D_008102B0
// and clears the global D_008106BD when it is 1); phases 0 and 1 then, when the effect
// bitfield is exactly 1, set its bit 1, build the scratchpad vector at D_700038A0 from
// self+0xB0, publish ctx+0x6C to 0x700038A4, spawn effect 0x80000016 aimed at self+0xC0
// and scale it by 5.0; and when the countdown float at ctx+0x48 has expired and bit 1 of
// ctx+0x81 is set, advance the sub-state, zero both timers, play sound 0x7E6 and start
// anim clip 8.  Phase 2 waits for bit 0x1000 of ctx+0x20 and then clears the actor's
// state/sub-state bytes.
// The `zi = 0; z = (float)zi;` staging before the clip-8 call is load-bearing: it is what
// makes mwcc emit `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12` for that call's FP arguments.
extern void anim_clip_init(char *, int, float, float);
extern void func_00102948(int *, char *);
extern void func_001E8B90(int *, float);
extern void func_001EFD90(int, int *, char *);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C040(char *, char *);
extern char D_008102B0[];
extern unsigned char D_008106BD;
extern int D_700038A0;

void func_00148520(char *a0, char *a1) {
    char *s1 = a0;
    char *s0 = a1;
    unsigned char st = *(unsigned char *)(a0 + 6);
    char v;
    int zi;
    float z;

    switch (st) {
    case 0:
        *(unsigned char *)(s1 + 6) = (unsigned char)(st + 1);
        *(int *)(s0 + 0x40) = 0x3F800000;
        *(char *)(s0 + 0x88) = 0;
        *(char *)(s0 + 0x83) = 0;
        *(int *)(s0 + 0x50) = 0;
        *(char *)(s0 + 0x8D) = (char)(*(char *)(s0 + 0x8D) & 1);
        anim_clip_init(s1, 7, 1.0f, 0.0f);
        if (*(char *)(s0 + 0x86) != 0) {
            *(char *)(s0 + 0x86) = 0;
            func_0021C040(D_008102B0, s1);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        /* fallthrough */
    case 1:
        v = *(char *)(s0 + 0x8D);
        if (v == 1) {
            *(char *)(s0 + 0x8D) = (char)(v | 2);
            func_00102948(&D_700038A0, s1 + 0xB0);
            *(float *)0x700038A4 = *(float *)(s0 + 0x6C);
            func_001EFD90(0x80000016, &D_700038A0, s1 + 0xC0);
            func_001E8B90(&D_700038A0, 5.0f);
        }
        if (*(float *)(s0 + 0x48) <= 0.0f && (*(char *)(s0 + 0x81) & 2)) {
            *(unsigned char *)(s1 + 6) = (unsigned char)(*(unsigned char *)(s1 + 6) + 1);
            *(int *)(s0 + 0x4C) = 0;
            *(int *)(s0 + 0x48) = 0;
            func_001FBD50(s1, 0x7E6, 0, 300.0f);
            zi = 0;
            z = (float)zi;
            anim_clip_init(s1, 8, 1.0f, z);
        }
        break;
    case 2:
        if (*(int *)(s0 + 0x20) & 0x1000) {
            *(char *)(s1 + 5) = 0;
            *(unsigned char *)(s1 + 6) = 0;
        }
        break;
    }
}
