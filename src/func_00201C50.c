// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.25% NEARMISS).
// Fixes read from the original instructions:
//  - The tail is two early returns, `if (pad & 0x30) { ...; return (pad & 0x10) ? 2 : 1; }
//    return 0;`, not a `ret` local preset to 0: the original tests the 0x30 mask in the
//    0x40 branch's delay slot (a plain branch, not branch-likely) and keeps a separate
//    return-0 path.
//  - func_0020CDA0 keeps its two-pointer declaration (see the note at the extern).
//
// Menu/pad handler: steps the scratchpad state byte at +0xD (0 -> 1 -> 2), draws a
// sprite via func_00207E40 at an x derived from arg1 and arg3[cursor index],
// toggles arg0[3] on the 0x2000 button, and returns 1 on the 0x40 button (confirm,
// resetting arg0 fields when arg0[3] was set), 2/1 on the 0x10/0x20 buttons, else 0.

extern unsigned char *D_70003B6C;                   /* PS2 scratchpad @ 0x70003B6C */

extern int float_to_int(float);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, unsigned long long t2);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
/* Defined (void) in src/func_0020CDA0.c, but the original sets a0 = g + 0xD and
   a1 = g before this call; declaring the two pass-through values reproduces it
   (the (void) spelling scores 99.70%). Same situation as func_0020B210. */
extern void func_0020CDA0(unsigned char *, unsigned char *);
extern void func_001AF470(unsigned char);
extern unsigned short D_00810E6A;
extern unsigned short D_00810E74;

int func_00201C50(unsigned char *arg0, int arg1, unsigned char *arg2, int *arg3) {
    unsigned char *g;
    unsigned char *p;
    int idx;
    int off;

    g = D_70003B6C;
    p = g + 0xD;
    switch (*(unsigned char *)(g + 0xD)) {
    case 0:
        *p = *p + 1;
        arg0[3] = 0;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x2000) {
            func_0020CDA0(p, g);
            g = D_70003B6C;
            g[0xD] = g[0xD] + 1;
            *(unsigned short *)(D_70003B6C + 0x1E) = 0xA;
            idx = *(unsigned short *)(D_70003B6C + 0x1C);
            off = ((arg1 + arg3[idx] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
            arg0[3] = 1 - arg0[3];
        } else {
            idx = *(unsigned short *)(D_70003B6C + 0x1C);
            off = ((arg1 + arg3[idx] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x8));
        }
        break;
    case 2: {
        int dec;
        dec = *(unsigned short *)(g + 0x1E) - 1;
        *(unsigned short *)(g + 0x1E) = (unsigned short) dec;
        if (!(dec & 0xFFFF)) {
            g = D_70003B6C;
            g[0xD] = g[0xD] - 1;
        }
        idx = *(unsigned short *)(D_70003B6C + 0x1C);
        off = ((arg1 + arg3[idx] * 0x18) >> 1) + 0x790;
        func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
        break;
    }
    }
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        if (arg0[3] == 1) {
            if (D_00810E6A == 7) {
                arg0[1] = 1;
            } else {
                arg0[1] = 0;
            }
            arg0[4] = 0;
            arg0[0] = 0;
            func_001AF470(arg0[0]);
            *(short *)(arg0 + 8) = 0;
            *(short *)(arg0 + 0xA) = 0;
            *(short *)0x70003B94 = 0;
            *(short *)0x70003B96 = 0;
        }
        arg0[3] = 0;
        return 1;
    }
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        arg0[3] = 0;
        return (D_00810E74 & 0x10) ? 2 : 1;
    }
    return 0;
}
