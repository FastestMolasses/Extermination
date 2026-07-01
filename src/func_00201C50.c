// NEARMISS func_00201C50  (vram 0x00201C50, 0x318 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see summary above
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int float_to_int(float);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, unsigned long long t2);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(unsigned char *, unsigned char *);
extern void func_001AF470(unsigned char);
extern unsigned short D_00810E6A;
extern unsigned short D_00810E74;

int func_00201C50(unsigned char *arg0, int arg1, unsigned char *arg2, int *arg3) {
    unsigned char *g;
    unsigned char *p;
    int idx;
    int off;
    int ret;

    g = *(unsigned char **)0x70003B6C;
    p = g + 0xD;
    switch (*(unsigned char *)(g + 0xD)) {
    case 0:
        *p = *p + 1;
        arg0[3] = 0;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x2000) {
            func_0020CDA0(p, g);
            g = *(unsigned char **)0x70003B6C;
            g[0xD] = g[0xD] + 1;
            *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1E) = 0xA;
            idx = *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C);
            off = ((arg1 + arg3[idx] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x10));
            arg0[3] = 1 - arg0[3];
        } else {
            idx = *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C);
            off = ((arg1 + arg3[idx] * 0x18) >> 1) + 0x790;
            func_00207E40(1, 0x88D0, float_to_int((float) off * 16.0f), 0x20, 0x20, 0x80808080, *(unsigned long long *)(arg2 + 0x8));
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
        idx = *(unsigned short *)(*(unsigned char **)0x70003B6C + 0x1C);
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
    ret = 0;
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        arg0[3] = 0;
        ret = (D_00810E74 & 0x10) ? 2 : 1;
    }
    return ret;
}
