// NEARMISS func_0019AD00  (vram 0x0019AD00, 0x2E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation + 3 scheduling artifacts (mwcc 2.3.3; body, all five loops, all 8 call setups, s-reg map s4/s3/s2/s1/s0 and both mode-2/mode-4 sections byte-identical). Residuals: (a) the CSE temp for (flags & 1) colors a1 where target uses v1, cascading into the path-B ternary regi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera track update, target-vector variant (twin of func_0019AFE0; same family
// as func_0019A6F0/func_0019B6C0). Seeds the scratchpad camera block D_70003190:
// eye = (obj->+0xB0, target[1], obj->+0xB8), goal[+0x10] = target[0..2] (also
// snapshotted to a stack copy), zeroes +0x1C/+0x2C(0x319C/31AC) and the lock ptr
// 0x700031D4, then advances the aim point D_700031A0 by 1% of the normalized
// (aim - eye) direction (func_001028D0=vec_sub, func_00102760=normalize,
// func_00103230=scale, func_001028B8=vec_add). flags&1 (+obj byte0 bit0): picks a
// lock via func_001A6440((flags&1)?0x40:0) when (obj[2]&0x1F)==0, else
// func_001A7280(); a lock is rejected if the half at +0x52 has bit1 set (global
// target for the 6440 path / obj for the 7280 path); on success and !(obj+0x52&1)
// copies +0x20 -> +0x10 (goal=locked) and sets mode 1. Always publishes obj[2]&0x1F
// to 0x7000324E. flags&2: stores obj->+0x14 to 0x70003254, mode 2 if func_0019FE50()
// returns 0. flags&4: mode 4 if func_0019CB60() returns 0. Then re-subtracts
// (aim - dir) into the scratch vec; if a mode won, restores goal from the entry
// snapshot and writes the goal delta to +0x30, applying the drift at 0x700031C0/C8
// to obj +0xB0/+0xB8 when flags bit31 is set; else clears 0x700031D0. Returns the
// mode, also stored to 0x700031D8.

extern void func_001028D0(void *, void *, void *);
extern void func_00102760(void *, void *);
extern void func_00103230(void *, void *, float);
extern void func_001028B8(void *, void *, void *);
extern int func_0019FE50();
extern int func_0019CB60();
extern int func_001A6440(int);
extern int func_001A7280();

extern float D_70003190;
extern float D_700031A0;

int func_0019AD00(char *arg0, float *arg1, int flags) {
    int f1;
    int ok;
    int mode;
    float *base;
    float sp60[4];
    float sp70[3];
    float *dst;
    float *loc;

    base = sp70;
    *(float *)0x70003190 = *(float *)(arg0 + 0xB0);
    mode = 0;
    *(float *)0x70003194 = arg1[1];
    ok = 0;
    *(float *)0x70003198 = *(float *)(arg0 + 0xB8);
    dst = &D_70003190;
    loc = base;
    do {
        float f = *arg1;
        ok += 1;
        *(float *)((char *)dst + 0x10) = f;
        *loc = f;
        arg1 += 1;
        dst += 1;
        loc += 1;
    } while (ok < 3);
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    func_001028D0(sp60, &D_700031A0, &D_70003190);
    func_00102760(sp60, sp60);
    func_00103230(sp60, sp60, 0.01f);
    func_001028B8(&D_700031A0, &D_700031A0, sp60);
    f1 = flags & 1;
    if (f1 && (*(unsigned char *)arg0 & 1)) {
        if (!(*(unsigned char *)(arg0 + 2) & 0x1F)) {
            ok = func_001A6440(f1 ? 0x40 : 0);
            if ((ok != 0) && (*(unsigned short *)(*(int *)0x700031D4 + 0x52) & 2)) {
                ok = 0;
            }
        } else {
            if ((f1 ? 0x40 : 0) == 0x40) {
                ok = func_001A7280();
            }
            if ((ok != 0) && !(*(unsigned short *)(arg0 + 0x52) & 2)) {
                ok = 0;
            }
        }
        if ((ok != 0) && !(*(unsigned short *)(arg0 + 0x52) & 1)) {
            int j = 0;
            float *p = &D_70003190;
            do {
                j += 1;
                *(float *)((char *)p + 0x10) = *(float *)((char *)p + 0x20);
                p += 1;
            } while (j < 3);
            mode = 1;
        }
    }
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (flags & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_0019FE50() == 0) {
            mode = 2;
        }
    }
    if ((flags & 4) && (func_0019CB60() == 0)) {
        mode = 4;
    }
    func_001028D0(&D_700031A0, &D_700031A0, sp60);
    if (mode != 0) {
        float *p = &D_70003190;
        int i = 0;
        do {
            i += 1;
            *(float *)((char *)p + 0x10) = *base;
            base += 1;
            *(float *)((char *)p + 0x30) = *(float *)((char *)p + 0x20) - *(float *)((char *)p + 0x10);
            p += 1;
        } while (i < 3);
        if (flags & 0x80000000) {
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
        }
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = mode;
    return mode;
}
