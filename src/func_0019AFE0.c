// NEARMISS func_0019AFE0  (vram 0x0019AFE0, 0x2E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Identical residual set to its twin func_0019AD00 (byte-for-byte the same body past the entry): (a) (flags & 1) CSE temp colored a1 vs target v1 + path-B ternary register permutation with one duplicated addiu 0x40; (b) two beqz slot-fill nops where target hoists the following lui; (c) tail lui-at ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera track update, explicit-eye variant (twin of func_0019AD00 — see that
// file for the shared semantics). Differences: the eye position comes from the
// caller-supplied pos vector (pos[0], tgt[1], pos[2]) instead of the object's
// +0xB0/+0xB8 fields, the goal is the tgt vector, and after the mode dispatch it
// ADDS the scratch vec into the eye block (func_001028B8 on D_70003190) instead
// of re-subtracting on the aim block. Everything else — the 1% aim smoothing, the
// flags&1 lock acquire (func_001A6440/func_001A7280 + the +0x52 bit1/bit0 vetoes),
// the flags&2 func_0019FE50 -> mode 2 and flags&4 func_0019CB60 -> mode 4 paths,
// the snapshot restore + delta write, the bit31 drift apply, and the 0x700031D8
// mode publish — is identical.

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

int func_0019AFE0(char *arg0, float *pos, float *tgt, int flags) {
    int f1;
    int ok;
    int mode;
    float *base;
    float sp60[4];
    float sp70[3];
    float *dst;
    float *loc;

    base = sp70;
    *(float *)0x70003190 = pos[0];
    mode = 0;
    *(float *)0x70003194 = tgt[1];
    ok = 0;
    *(float *)0x70003198 = pos[2];
    dst = &D_70003190;
    loc = base;
    do {
        float f = *tgt;
        ok += 1;
        *(float *)((char *)dst + 0x10) = f;
        *loc = f;
        tgt += 1;
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
    func_001028B8(&D_70003190, &D_70003190, sp60);
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
