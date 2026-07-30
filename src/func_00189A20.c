// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SEMANTICS: scripted camera-shake / push state-machine step (state byte p[5], 6
// states, countdown timer in the short at p[0x28]).
// Publishes the current source transform into p[0xD0] and hands it to func_001C9610
// for entity p[0xC]; force-resets to state 0 unless D_008102B4 == 1. Then:
//   0: wait for global mode 0x275==5 and 0x1F0==0x33, advance, arm the 8-tick timer
//      and clear the 0x70..0x7C accumulator.
//   1: tick the timer down; on expiry advance. Otherwise ramp p[0x70] up by
//      0.1573875, rotate it into world space and add it onto the camera at +0xC0.
//   2: below the 19.0 timer, advance and re-arm the 8-tick timer; add every frame.
//   3: mirror of state 1 with the ramp going down.
//   4: advance once bit 0x1000 of the global flag word at 0x200 is set.
//   5: bail back to state 0 once the global mode 0x1F0 leaves 0x33.

extern unsigned char D_008102B0[];
extern unsigned char D_008102B4[8];
extern int D_008103D0[2];
extern int D_700038A0[4];
extern char *D_00275B40;

extern int copy_qw4();
extern int func_001C9610();
extern int func_001026A0();

void func_00189A20(unsigned char *p)
{
    unsigned char *g;
    int st;
    short t;
    char *q;

    g = D_008102B0;
    copy_qw4(p + 0xD0, D_008103D0[0] + 0x90);
    func_001C9610(D_00275B40, p[0xC], p + 0xD0);
    if (D_008102B4[0] != 1) {
        p[5] = 0;
    }
    st = p[5];
    switch (st) {
    case 0:
        if (g[0x275] == 5) {
            if (g[0x1F0] == 0x33) {
                *(volatile unsigned char *)(p + 5) = *(volatile unsigned char *)(p + 5) + 1;
                *(short *)(p + 0x28) = 8;
                *(int *)(p + 0x70) = 0;
                *(int *)(p + 0x74) = 0;
                *(int *)(p + 0x78) = 0;
                *(int *)(p + 0x7C) = 0;
            }
        }
        break;
    case 1:
        t = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = t - 1;
        if (t == 0) {
            *(volatile unsigned char *)(p + 5) = *(volatile unsigned char *)(p + 5) + 1;
        } else {
            *(float *)(p + 0x70) += 0.1573875f;
            func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC0) += *(volatile float *)0x700038A0;
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC4) += *(volatile float *)0x700038A4;
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC8) += *(volatile float *)0x700038A8;
        }
        break;
    case 2:
        if (*(float *)(g + 0x3C) <= 19.0f) {
            p[5] = st + 1;
            *(short *)(p + 0x28) = 8;
        }
        func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
        q = *(char **)(D_00275B40 + 4);
        *(float *)(q + 0xC0) += *(volatile float *)0x700038A0;
        q = *(char **)(D_00275B40 + 4);
        *(float *)(q + 0xC4) += *(volatile float *)0x700038A4;
        q = *(char **)(D_00275B40 + 4);
        *(float *)(q + 0xC8) += *(volatile float *)0x700038A8;
        break;
    case 3:
        t = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = t - 1;
        if (t == 0) {
            *(volatile unsigned char *)(p + 5) = *(volatile unsigned char *)(p + 5) + 1;
        } else {
            *(float *)(p + 0x70) -= 0.1573875f;
            func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC0) += *(volatile float *)0x700038A0;
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC4) += *(volatile float *)0x700038A4;
            q = *(char **)(D_00275B40 + 4);
            *(float *)(q + 0xC8) += *(volatile float *)0x700038A8;
        }
        break;
    case 4:
        if (*(int *)(g + 0x200) & 0x1000) {
            p[5] = st + 1;
        }
        break;
    case 5:
        if (g[0x1F0] != 0x33) {
            p[5] = 0;
        }
        break;
    }
}
