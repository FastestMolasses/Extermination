// NEARMISS func_0022B420  (vram 0x0022B420, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Rewritten 2026-10-27 from the
// byte-matched twin func_00201720 (switch(state) with case 0 falling into case 1). Fixes read from the original:
// the state-2 countdown is the scratchpad record's +0x1E (not arg3's); the back/cancel return is 2 when
// D_00810E74&0x10 is set, else 1 (was inverted); the confirm/cancel tails are ternary returns; the original keeps
// arg0/arg1 in a0/a1 and the record pointers in a2/a3 at the func_0020CDA0 call, i.e. it is passed four arguments
// (src/func_0020CDA0.c defines it (void); a 4-parameter definition would document that).
// Remaining differences: one instruction (mwcc re-copies arg1 from its saved register into a1 in the call's delay
// slot; the original leaves a1 untouched) and the post-call record reload's register (a0 vs a2).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Menu toggle-row driver (sibling of func_00201720). g = the active menu-row record at the
// scratchpad pointer 0x70003B6C, p = &g[0xD] (its state byte). State 0 bumps the state and
// clears g[0x13], then falls into state 1: if D_00810E74&0x2000 (row activated) run
// func_0020CDA0, bump the state, arm a 10-frame blink timer at g+0x1E, draw the glyph with the
// highlighted colour (arg2+0x10) and toggle g[0x13]; otherwise draw it with the dim colour
// (arg2+0x8). State 2 ticks the timer down (dropping the state back when it reaches 0) and
// draws highlighted. Glyph x = ((arg1 + arg3[g->row]*24) >> 1) + 0x790, scaled by 16.
// Tail: confirm (0x40) -> func_0020CD40, return 3 if g[0x13]==1 else 1; back/cancel (0x30) ->
// func_0020CD60, return 2 if 0x10 else 1; otherwise 0.

extern unsigned char *D_70003B6C;                   /* PS2 scratchpad @ 0x70003B6C */

extern int float_to_int(float);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, unsigned long long t2);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(int, int, unsigned char *, unsigned char *);
extern unsigned short D_00810E74;

int func_0022B420(int arg0, int arg1, unsigned char *arg2, int *arg3) {
    unsigned char *g;
    unsigned char *p;
    int off;
    int ret;

    g = D_70003B6C;
    p = g + 0xD;
    switch (*(unsigned char *)(g + 0xD)) {
    case 0:
        *p = *p + 1;
        D_70003B6C[0x13] = 0;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x2000) {
            func_0020CDA0(arg0, arg1, p, g);
            g = D_70003B6C;
            g[0xD] = g[0xD] + 1;
            *(unsigned short *)(D_70003B6C + 0x1E) = 0xA;
            off = ((arg1 + arg3[*(unsigned short *)(D_70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
            g = D_70003B6C;
            g[0x13] = 1 - g[0x13];
        } else {
            off = ((arg1 + arg3[*(unsigned short *)(D_70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
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
        off = ((arg1 + arg3[*(unsigned short *)(D_70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
        func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
        break;
    }
    }
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        return (D_70003B6C[0x13] == 1) ? 3 : 1;
    }
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        return (D_00810E74 & 0x10) ? 2 : 1;
    }
    return 0;
}
