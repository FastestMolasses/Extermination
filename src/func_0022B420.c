// NEARMISS func_0022B420  (vram 0x0022B420, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/logic fully recovered (state dispatch 0/1/2, screen-position compute via float_to_int(16.0f*...), func_00207E40 7-arg call, tail flag-gated return). Residual is the prologue callee-saved-register assignment ORDER (s0/s1/s2 <- a3/a2/a1 emission order) plus the state==2 block's placement relat...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int float_to_int(float);
extern int func_00207E40(int, int, int, int, int, int, long long);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void *, void *);
extern unsigned short D_00810E74;

int func_0022B420(int arg0, char *arg1, char *arg2, int *arg3) {
    char *p;
    unsigned char state;
    int idx;
    int v0;
    float f;
    int result;

    p = *(char **)0x70003B6C;
    state = *(unsigned char *)(p + 0xD);
    if (state == 2) {
        goto state2;
    }
    if (state == 1 || state == 0) {
        if (state == 0) {
            p = *(char **)0x70003B6C;
            *(unsigned char *)(p + 0xD) = *(unsigned char *)(p + 0xD) + 1;
            p = *(char **)0x70003B6C;
            *(unsigned char *)(p + 0x13) = 0;
        }
        if (D_00810E74 & 0x2000) {
            func_0020CDA0(p + 0xD, p);
            p = *(char **)0x70003B6C;
            *(unsigned char *)(p + 0xD) = *(unsigned char *)(p + 0xD) + 1;
            p = *(char **)0x70003B6C;
            *(unsigned short *)(p + 0x1E) = 0xA;
            p = *(char **)0x70003B6C;
            idx = *(unsigned short *)(p + 0x1C);
            v0 = arg3[idx];
            v0 = ((v0 * 3) << 3);
            v0 = (int)(arg1 + v0);
            v0 = (v0 >> 1) + 0x790;
            f = 16.0f * (float) v0;
            result = float_to_int(f);
            func_00207E40(1, 0x88D0, result, 0x20, 0x20, (int) 0x80808080, *(long long *)(arg2 + 0x10));
            p = *(char **)0x70003B6C;
            *(unsigned char *)(p + 0x13) = 1 - *(unsigned char *)(p + 0x13);
        } else {
            idx = *(unsigned short *)(p + 0x1C);
            v0 = arg3[idx];
            v0 = ((v0 * 3) << 3);
            v0 = (int)(arg1 + v0);
            v0 = (v0 >> 1) + 0x790;
            f = 16.0f * (float) v0;
            result = float_to_int(f);
            func_00207E40(1, 0x88D0, result, 0x20, 0x20, (int) 0x80808080, *(long long *)(arg2 + 0x8));
        }
    }
    goto tail;

state2:
    {
        unsigned short cnt;

        cnt = *(unsigned short *)((char *)arg3 + 0x1E) - 1;
        *(unsigned short *)((char *)arg3 + 0x1E) = cnt;
        if (cnt == 0) {
            p = *(char **)0x70003B6C;
            *(unsigned char *)(p + 0xD) = *(unsigned char *)(p + 0xD) - 1;
        }
        p = *(char **)0x70003B6C;
        idx = *(unsigned short *)(p + 0x1C);
        v0 = arg3[idx];
        v0 = ((v0 * 3) << 3);
        v0 = (int)(arg1 + v0);
        v0 = (v0 >> 1) + 0x790;
        f = 16.0f * (float) v0;
        result = float_to_int(f);
        func_00207E40(1, 0x88D0, result, 0x20, 0x20, (int) 0x80808080, *(long long *)(arg2 + 0x10));
    }

tail:
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        v0 = 1;
        p = *(char **)0x70003B6C;
        if (*(unsigned char *)(p + 0x13) == 1) {
            v0 = 3;
        }
        return v0;
    }
    v0 = 0;
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        v0 = (D_00810E74 & 0x10) ? 1 : 2;
    }
    return v0;
}
