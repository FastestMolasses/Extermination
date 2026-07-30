// NEARMISS func_00107CF0  (vram 0x00107CF0, 0x194 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.02% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. 104/105 instructions byte-identical; the ONLY diff is the 4th call's argument copy `daddu a0,s2,zero`, which our build schedules one slot earlier (between `dsll v0,v0,31` and `dsra32 v0,v0,0`) than the target (which emits it right after the dsra32). Pure sched2 ready-li...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char *D_002412F4;

extern long func_00125F48(long a, long b);

void func_00107CF0(unsigned char *ctx, long *out_time, long *out_a, long *out_b)
{
    unsigned char *dev;
    long t;
    long frac;
    long first;
    long p;
    long q;
    long r;
    int base;
    int lo;
    int bit;
    int cnt;
    int adj;

    dev = *(unsigned char **)(D_002412F4 + 0x40);
    if (*(int *)(dev + 0x70) != 0) {
        t = *(long *)(ctx + 0x18);
        if (t >= 0) {
            *out_time = t;
        } else {
            base = *(int *)(dev + 0x80);
            if (base < 0) {
                *out_time = t;
            } else {
                lo = (int)*(long *)(dev + 0x88);
                bit = lo & 1;
                first = func_00125F48(bit, *(long *)(dev + 0x78) & 1);
                cnt = *(int *)(dev + 0x90);
                adj = (int)func_00125F48(first, cnt & 1);
                frac = func_00125F48(*(long *)(dev + 0x78), lo);
                *out_time = base + ((int)((frac << 31) >> 32) + adj);
                if (func_00125F48(bit, *(long *)(dev + 0x78) & 1) != 0) {
                    *(int *)(dev + 0x90) = cnt + 1;
                }
            }
        }
    } else {
        *out_time = *(long *)(ctx + 0x18);
    }

    if (*(int *)(dev + 0xF8) == 2) {
        long pend = *(long *)(dev + 0xF0);
        if (pend >= 0) {
            *out_time = pend;
            *(int *)(dev + 0xF8) = 0;
            *(long *)(dev + 0xF0) = -1;
        }
    }

    q = ((long)*(int *)(ctx + 0x40) << 5) | ((long)*(int *)(ctx + 0x3C) << 6);
    *out_a = *(long *)(ctx + 0x20);
    p = ((long)*(int *)(ctx + 0x34) << 8) | *(int *)(ctx + 0x2C);
    r = ((long)*(int *)(ctx + 0x38) << 7) | ((long)*(int *)(ctx + 0x30) << 3);
    *out_b = (p | q) | r;
}
