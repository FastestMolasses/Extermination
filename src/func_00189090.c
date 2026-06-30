// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-frame state driver for the player actor (D_008102B0). First copies bone
// transform 0x90 from the template (D_008103D0[0]) into each of arg0->numbones
// (arg0+0xC) bone nodes (D_00275B40[i]). If the actor's field+4 != 1, resets the
// state byte arg0+5 to 0. Then dispatches on the state byte (0..3, case 2 falls
// through into case 3):
//   0: once the global handshake (D_008102B0+0x275==3, +0x274!=0, D_00810CA8!=0)
//      is satisfied, advance the state.
//   1: when the actor clock (D_008102B0+0x3C) passes 21.0, advance, kick
//      func_001FBD50(actor,0x5DF,0,300.0f), and zero arg0+0x70..0x7C and arg0+0x28.
//   2: when the clock passes 12.0, advance, snapshot D_700036A0 from bone0,
//      build a matrix into D_700036D0, and func_001F4010(7,&D_700036A0); falls
//      into case 3.
//   3: when the clock passes 2.0, reset state to 0; otherwise integrate the
//      0x70 accumulator by D_0024A360[arg0+0x28], push it through func_001026A0
//      into D_700038A0, add that delta into bone1 (D_00275B40[1]) +0xC0..0xC8,
//      and bump the 0x28 index.
extern void copy_qw4(char *dst, char *src);
extern int func_001026A0(void *dst, int src, void *m);
extern void func_001F4010(int a, void *b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char **D_00275B40;
extern char *D_008103D0[16];
extern char D_008102B0[];
extern short D_00810CA8[8];
extern char D_700036A0[];
extern char D_700036D0[];
extern float D_700038A0[3];
extern float D_0024A360[];
extern char D_0024A330[];

void func_00189090(char *arg0) {
    char *self = D_008102B0;
    int i;
    int j;
    char *node;
    char *bone;

    i = 0;
    j = 0;
    for (; i < *(unsigned char *)(arg0 + 0xC); i++) {
        node = *(char **)((char *)D_00275B40 + j);
        copy_qw4(node + 0x90, D_008103D0[0] + 0x90);
        j += 4;
    }

    if (*(unsigned char *)(self + 4) != 1) {
        *(unsigned char *)(arg0 + 5) = 0;
    }

    switch (*(unsigned char *)(arg0 + 5)) {
    case 0:
        if (*(unsigned char *)(self + 0x275) == 3 &&
            *(unsigned char *)(self + 0x274) != 0 &&
            D_00810CA8[0] != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        }
        break;
    case 1:
        if (*(float *)(self + 0x3C) <= 21.0f) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            func_001FBD50(D_008102B0, 0x5DF, 0, 300.0f);
            *(float *)(arg0 + 0x70) = 0.0f;
            *(int *)(arg0 + 0x74) = 0;
            *(int *)(arg0 + 0x78) = 0;
            *(int *)(arg0 + 0x7C) = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        break;
    case 2:
        if (*(float *)(self + 0x3C) <= 12.0f) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            copy_qw4(D_700036A0, *(char **)D_00275B40 + 0x90);
            func_001026A0(D_700036D0, (int)(*(char **)D_00275B40 + 0x90), &D_0024A330);
            func_001F4010(7, D_700036A0);
        }
        /* fallthrough */
    case 3:
        if (*(float *)(self + 0x3C) <= 2.0f) {
            *(unsigned char *)(arg0 + 5) = 0;
            break;
        }
        *(float *)(arg0 + 0x70) =
            *(float *)(arg0 + 0x70) + D_0024A360[*(short *)(arg0 + 0x28)];
        func_001026A0(D_700038A0, (int)(*(char **)D_00275B40 + 0x90), arg0 + 0x70);
        bone = *(char **)((char *)D_00275B40 + 4);
        *(float *)(bone + 0xC0) = *(float *)(bone + 0xC0) + *(float *)0x700038A0;
        bone = *(char **)((char *)D_00275B40 + 4);
        *(float *)(bone + 0xC4) = *(float *)(bone + 0xC4) + *(float *)0x700038A4;
        bone = *(char **)((char *)D_00275B40 + 4);
        *(float *)(bone + 0xC8) = *(float *)(bone + 0xC8) + *(float *)0x700038A8;
        *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
        break;
    }
}
