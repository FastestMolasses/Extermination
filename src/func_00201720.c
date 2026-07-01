// NEARMISS func_00201720  (vram 0x00201720, 0x52C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single root-cause delay-slot-fill artifact: target leaves the unhandled-switch-value branch's delay slot as nop; mwcc 2.3.3 hoists the next block's `lui at,%hi(0x70003B6C)` load into it (cross-block speculative hoist not present in the original build). This shifts every later branch target addres...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Menu-cursor-blink/select driver. `g` = the active menu-row context object at the fixed
// scratch pointer 0x70003B6C; `p` = &g[0xD] (its animation/nest-depth state byte).
// arg0 = per-row selection state (byte 1 = primary column selector, byte 4 = secondary
// column selector), arg1 = a pixel/layout base offset, arg2 = a per-row color-table
// pointer (u64 packed colors at +0x8 "dim" and +0x10 "highlighted"), arg3 = a per-row
// int lookup table used as a 0/non-zero "which column" selector via g's row index +0x1C.
// Dispatches on g's state byte g[0xD] (values 0/1/2):
//  state 0 (falls into 1): bump the nest counter *p, latch g[0x13] from the currently
//   selected column (arg0[1] or arg0[4], chosen by the row's column selector).
//  state 1: if the "row changed" flag D_00810E74&0x2000 is set, run the row-transition
//   sound/anim func_0020CDA0(p, g), bump *p again, arm a ~10-frame blink timer at g+0x1E,
//   toggle the selection bit on whichever column arg3[idx] selects (playing a click sound
//   func_001B61C0 only when toggling column 1 to "on"), then issue the highlighted glyph
//   draw func_00207E40 (register set 0x88D0 for column 1 / 0x8A40 for column 2) with the
//   "highlighted" color u64 from arg2+0x10. If the row didn't just change, issue the same
//   glyph draw but with the "dim" color u64 from arg2+0x8 instead.
//  state 2: tick the blink timer at g+0x1E down by one; on underflow, decrement the nest
//   counter g[0xD] back down. Always redraw the glyph with the "highlighted" color
//   (arg2+0x10), selecting register set 0x88D0/0x8A40 the same way.
// Tail (always runs): if the row's column selector says column 1 (arg3[idx]==0) and we
//  are not in the special "difficulty 4, mode 7" case, latch arg0[1] from g[0x13] (or 0
//  in that special case) and return 1 (still active). Otherwise, if the "confirm" flag
//  D_00810E74&0x40 is set, run the accept sound func_0020CD40 and return 1. Otherwise, if
//  the "cancel/back" flags D_00810E74&0x30 are set, run func_0020CD60, latch the selected
//  column's byte from g[0x13] into arg0[1] or arg0[4], and return 2 if the "back" bit
//  D_00810E74&0x10 is additionally set, else return 1. Otherwise return 0 (idle).
extern int float_to_int(float);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, unsigned long long t2);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(unsigned char *, unsigned char *);
extern unsigned char D_00810E50;
extern unsigned short D_00810E6A;
extern unsigned short D_00810E74;

int func_00201720(unsigned char *arg0, int arg1, unsigned char *arg2, int *arg3) {
    unsigned char *g;
    unsigned char *p;
    int idx;
    int off;
    int ret;

    g = *(unsigned char **)0x70003B6C;
    p = g + 0xD;
    switch (*(unsigned char *)(g + 0xD)) {
    case 0: {
        unsigned char *g2;
        *p = *p + 1;
        g2 = *(unsigned char **)0x70003B6C;
        idx = *(unsigned short *)(g2 + 0x1C);
        if (arg3[idx] == 0) {
            g2[0x13] = arg0[1];
        } else {
            g2[0x13] = arg0[4];
        }
        /* fallthrough */
    }
    case 1:
        if (D_00810E74 & 0x2000) {
            func_0020CDA0(p, g);
            g = *(unsigned char **)0x70003B6C;
            g[0xD] = g[0xD] + 1;
            *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1E) = 0xA;
            idx = *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C);
            if (arg3[idx] == 0) {
                arg0[1] = 1 - arg0[1];
                if (arg0[1] == 1) {
                    func_001B61C0(1, 0xFF, 4, 1);
                }
            } else {
                arg0[4] = 1 - arg0[4];
            }
            if (arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] == 0) {
                off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
                func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
            } else {
                off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
                func_00207E40(1, 0x8A40, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
            }
        } else {
            if (arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] == 0) {
                off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
                func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x8));
            } else {
                off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
                func_00207E40(1, 0x8A40, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x8));
            }
        }
        break;
    case 2: {
        int dec;
        dec = *(unsigned short *)(g + 0x1E) - 1;
        *(unsigned short *)(g + 0x1E) = (unsigned short) dec;
        if (!(dec & 0xFFFF)) {
            g = *(unsigned char **)0x70003B6C;
            g[0xD] = g[0xD] - 1;
        }
        if (arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] == 0) {
            off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
        } else {
            off = ((arg1 + arg3[*(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C)] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x8A40, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
        }
        break;
    }
    }
    g = *(unsigned char **)0x70003B6C;
    idx = *(unsigned short *)(g + 0x1C);
    if (arg3[idx] == 0 && (D_00810E6A != 7 || D_00810E50 != 4)) {
        if (D_00810E50 != 4) {
            arg0[1] = g[0x13];
        } else {
            arg0[1] = 0;
        }
        return 1;
    }
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        return 1;
    }
    ret = 0;
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        g = *(unsigned char **)0x70003B6C;
        idx = *(unsigned short *)(g + 0x1C);
        if (arg3[idx] == 0) {
            arg0[1] = g[0x13];
        } else {
            arg0[4] = g[0x13];
        }
        ret = 1;
        if (D_00810E74 & 0x10) {
            return 2;
        }
        return ret;
    }
    return ret;
}
