// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state update tick for an actor (arg0). Builds two work vectors at the
// global scratch buffers D_700038A0/D_700038B0, computes two derived floats
// stored at +0x224/+0x228 of the actor (p = arg0+0x1F0), dispatches on the
// actor's state byte (arg0+4) to a per-state handler, then decrements a set of
// per-actor timers (bytes at p+0x58/+0x5E, halfwords at p+0x52/+0x56/+0x50)
// down to zero, and finishes with two trailing housekeeping calls.

extern void func_00102948(void *, void *);
extern void func_00133FD0(char *, char *);
extern void func_00134090(char *, char *);
extern void func_00136140(char *, char *);
extern void func_00136600(char *, char *);
extern void func_001B0D80(char *);
extern float func_001B1240(void *, float, float);
extern float func_001B15D0(void *, void *);
extern void func_001F91C0(char *);
extern unsigned char D_00810360;
extern unsigned char D_700038A0;
extern unsigned char D_700038B0;

void func_00133E30(char *arg0) {
    char *p;
    unsigned char st;
    char cb;
    short hw;

    p = arg0 + 0x1F0;
    func_00102948(&D_700038A0, arg0 + 0xB0);
    func_00102948(&D_700038B0, &D_00810360);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)(p + 0x34) = func_001B15D0(&D_700038A0, &D_700038B0);
    *(float *)(p + 0x38) = func_001B1240(&D_700038A0, *(float *)0x700038B0, *(float *)0x700038B8);
    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        func_00133FD0(arg0, p);
        break;
    case 1:
        func_00134090(arg0, p);
        break;
    case 2:
        func_00136140(arg0, p);
        break;
    case 3:
        func_00136600(arg0, p);
        return;
    }
    cb = *(char *)(p + 0x58);
    if (cb != 0) {
        *(char *)(p + 0x58) = cb - 1;
    }
    cb = *(char *)(p + 0x5E);
    if (cb != 0) {
        *(char *)(p + 0x5E) = cb - 1;
    }
    hw = *(short *)(p + 0x52);
    if (hw != 0) {
        *(short *)(p + 0x52) = hw - 1;
    }
    hw = *(short *)(p + 0x56);
    if (hw != 0) {
        *(short *)(p + 0x56) = hw - 1;
    }
    hw = *(short *)(p + 0x50);
    if (hw != 0) {
        *(short *)(p + 0x50) = hw - 1;
    }
    func_001F91C0(arg0);
    func_001B0D80(arg0);
}
