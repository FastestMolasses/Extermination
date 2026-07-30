// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SEMANTICS: cutscene/camera state-machine step (state byte at p[5], 6 states).
// Copies a source transform into each of p[0xC] linked entries, force-resets the
// state to 0 unless the global gate D_008102B0[4] == 1, then runs the state switch:
//   0: wait for global mode 0x275==1 and 0x1F0==0x33, then advance and clear the
//      0x70..0x7C vec4 accumulator.
//   1: advance once the current 0x20C id matches D_00248B9A or D_00248C7A.
//   2: below the 52.0 timer, advance + seed the D_700036A0/D_700036D0 work vectors;
//      every frame ramp p[0x70] up by 0.1 and push it through the camera helpers.
//   3: below the 7.0 timer, advance; push the accumulator every frame.
//   4: below the 1.0 timer, advance; otherwise ramp p[0x70] down by 0.1 and push.
//   5: bail back to state 0 once the global mode 0x1F0 leaves 0x33.

extern unsigned char D_008102B0[];
extern int D_008103D0[2];
extern short D_00248B9A[4];
extern short D_00248C7A[4];
extern int D_0024A310[4];
extern int D_700036A0[4];
extern int D_700036D0[4];
extern int D_700038A0[4];
extern char *D_00275B40;

extern int copy_qw4();
extern int func_001026A0();
extern int func_001028B8();
extern int func_001F4010();

void func_00189330(unsigned char *p)
{
    unsigned char *g;
    int i;
    int off;
    int st;
    int t;

    g = D_008102B0;
    i = 0;
    off = 0;
    while (i < (int)p[0xC]) {
        copy_qw4(*(int *)(D_00275B40 + off) + 0x90, D_008103D0[0] + 0x90);
        off += 4;
        i += 1;
    }
    if (g[4] != 1) {
        p[5] = 0;
    }
    st = p[5];
    switch (st) {
    case 0:
        if (g[0x275] == 1) {
            if (g[0x1F0] == 0x33) {
                *(volatile unsigned char *)(p + 5) = *(volatile unsigned char *)(p + 5) + 1;
                *(int *)(p + 0x70) = 0;
                *(int *)(p + 0x74) = 0;
                *(int *)(p + 0x78) = 0;
                *(int *)(p + 0x7C) = 0;
            }
        }
        break;
    case 1:
        if (*(short *)(g + 0x20C) == D_00248B9A[0] ||
            *(short *)(g + 0x20C) == D_00248C7A[0]) {
            *(volatile unsigned char *)(p + 5) = *(volatile unsigned char *)(p + 5) + 1;
        }
        break;
    case 2:
        if (*(float *)(g + 0x3C) <= 52.0f) {
            p[5] = st + 1;
            copy_qw4(D_700036A0, *(int *)D_00275B40 + 0x90);
            func_001026A0(D_700036D0, *(int *)D_00275B40 + 0x90, D_0024A310);
            func_001F4010(8, D_700036A0);
        }
        *(float *)(p + 0x70) += 0.1f;
        func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
        t = *(int *)(D_00275B40 + 4) + 0xC0;
        func_001028B8(t, t, D_700038A0);
        break;
    case 3:
        if (*(float *)(g + 0x3C) <= 7.0f) {
            p[5] = st + 1;
        }
        func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
        t = *(int *)(D_00275B40 + 4) + 0xC0;
        func_001028B8(t, t, D_700038A0);
        break;
    case 4:
        if (*(float *)(g + 0x3C) <= 1.0f) {
            p[5] = st + 1;
        } else {
            *(float *)(p + 0x70) -= 0.1f;
            func_001026A0(D_700038A0, *(int *)D_00275B40 + 0x90, p + 0x70);
            t = *(int *)(D_00275B40 + 4) + 0xC0;
            func_001028B8(t, t, D_700038A0);
        }
        break;
    case 5:
        if (g[0x1F0] != 0x33) {
            p[5] = 0;
        }
        break;
    }
}
