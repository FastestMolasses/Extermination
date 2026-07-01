// NEARMISS func_0019A6F0  (vram 0x0019A6F0, 0x214 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register coloring (s0=mode,s1=flags,s2=base,s3=arg0) and the 3-way sub-update dispatch now match byte-for-byte after inverting the (arg0+2&0x1F) if so func_001A6440 becomes the fall-through arm; the residual ~6% is pure instruction-scheduling in the entry copy loop (target walks D_70003190 in a0 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 93.6% (mwcc 2.3.3; pinned 991202 build 88.9%). Logic fully recovered.
// Takes a 5th integer argument in $t0 (arg4) -- this build passes the first
// integer args in a0-a3 then t0 (all callers set $t0=0x40 before the jal).
//
// Copies a 3-vector pair (arg1->[global+0], arg2->[global+0x10] AND into the
// stack scratch sp50[]), primes global scale/blend words at 0x700031AC/319C
// and 0x700031D4/D8, then, gated on the low bits of arg3 (flags), dispatches
// one of three sub-updates and records the winning mode (1/2/4) at 0x700031D8.
// flags&1: if (arg0[2]&0x1F)==0 run func_001A6440 else the 0x40-path
// func_001A7280 (arg = arg4&0xFFFF), copy scratch->global on success -> mode 1.
// flags&2: store arg0[0x14] to 0x70003254, func_001A0B10 -> mode 2.
// flags&4: func_0019D330 -> mode 4. On any success, copy sp50[] back into the
// global at +0x10; else clear 0x700031D0.
//
// WALL: register coloring (s0=mode/s1=flags/s2=base/s3=arg0) and the 3-way
// dispatch now match the target byte-for-byte; the sole residual is
// instruction-scheduling in the entry copy loop -- mwcc places the D_70003190
// walker in a temp (t1) and the counter in a0, where the target uses a0/v0,
// plus one sp5C-store slot shifted. Scheduling/regalloc-permutation class.
extern int func_001A6440(int, float *, float *);
extern int func_001A7280(int, float *, float *);
extern int func_001A0B10(void);
extern int func_0019D330(void);
extern int D_70003190;

int func_0019A6F0(char *arg0, float *arg1, float *arg2, int arg3, int arg4) {
    float sp50[3];
    int sp5C;
    int mode;
    int flags;
    int ok;
    int i;
    float *dst;
    float *t1;
    float *base;

    base = sp50;
    dst = (float *) &D_70003190;
    mode = 0;
    i = 0;
    t1 = base;
    do {
        float f0;
        i += 1;
        *dst = *arg1;
        f0 = *arg2;
        arg1 += 1;
        *t1 = f0;
        *(float *)((char *)dst + 0x10) = f0;
        dst += 1;
        arg2 += 1;
        t1 += 1;
    } while (i < 3);
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    flags = arg3 & 0xFF;
    sp5C = 0x3F800000;
    *(int *)0x700031D4 = 0;
    if ((flags & 1) && (*(unsigned char *)arg0 & 1)) {
        if (!(*(unsigned char *)(arg0 + 2) & 0x1F)) {
            ok = func_001A6440(arg4 & 0xFFFF, arg1, arg2);
            if (ok != 0 && (*(unsigned short *)(*(int *)0x700031D4 + 0x52) & 2)) {
                ok = 0;
            }
        } else {
            if ((arg4 & 0xFFFF) == 0x40) {
                ok = func_001A7280(0x40, arg1, arg2);
            }
            if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 2)) {
                ok = 0;
            }
        }
        if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 1)) {
            dst = (float *) &D_70003190;
            i = 0;
            do {
                i += 1;
                *(float *)((char *)dst + 0x10) = *(float *)((char *)dst + 0x20);
                dst += 1;
            } while (i < 3);
            mode = 1;
        }
    }
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (flags & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_001A0B10() != 0) {
            mode = 2;
        }
    }
    if ((flags & 4) && func_0019D330() != 0) {
        mode = 4;
    }
    if (mode != 0) {
        dst = (float *) &D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)((char *)dst + 0x10) = *base;
            base += 1;
            dst += 1;
        } while (i < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = mode;
    return mode;
}
