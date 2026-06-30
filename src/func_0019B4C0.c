// NEARMISS func_0019B4C0  (vram 0x0019B4C0, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// idiom-13 const-store delay-slot NOP x2 (else-store 0x700031D0 + flags&0x80000000 guard, target NOP vs mwcc-filled global lui) PLUS one permuter-class FP add-operand-order/register-coloring permutation in the final a0+0xB0 accumulate loop (target add.s f0,f0,f1 vs mwcc add.s f0,f1,f0; flipping ope...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: sibling of func_0019B2C0 but driven by two vectors (a1 = target, a2 = origin) and an actor a0.
// Loop builds the global scratch transform: D_70003190[i] = a1[i]-a2[i]; D_700031A0[i] = a1[i]; tmp[i] = a1[i]
// (i=0..2). Zeroes scratch 0x700031AC/0x7000319C/0x700031D4; runs the matrix helpers
// func_001028D0(mat,D_700031A0,D_70003190) / func_00102760 / func_00103230(.,.,0.01f) /
// func_001028D0(D_70003190,D_70003190,mat). Stores (a0[2] & 0x1F) as a short at 0x7000324E. Mode select:
// if (flags & 2) stores *(int*)(a0+0x14) at 0x70003254 and probes func_001A06A0() (mode 2 on zero); if
// (flags & 4) probes func_0019CF50() (mode 4). Calls func_001028B8(D_70003190,D_70003190,mat). If a mode
// was chosen: D_70003190[4..6]=tmp, D_70003190[12..14]=[8..10]-[4..6], and if flags<0 (bit 0x80000000)
// accumulates the delta D_70003190[12+i] into the actor's float triple at a0+0xB0. Otherwise clears
// 0x700031D0. Stores mode at 0x700031D8 and returns it.
//
// NOTE: the first func_001028D0 takes the SEPARATE global symbol D_700031A0 (not D_70003190+4) as its
// middle arg — using the +4 addend would mis-match the reloc symbol; declare D_700031A0 as its own extern.
//
// WALL: (1) idiom-13 const-store delay-slot NOP x2 — same shape as func_0019B2C0 (the else-store at
// 0x700031D0 and the flags&0x80000000 guard; target NOP vs mwcc-filled global `lui at`). (2) In the final
// `a0+0xB0 += D_70003190[12+i]` loop the target colors D into $f1, the field into $f0, and emits
// `add.s f0,f0,f1` (field+D); mwcc reaches the matching load coloring with `p[12] + f` but then emits the
// commuted `add.s f0,f1,f0`. Forcing the other operand order flips the load coloring instead. Permuter-class
// FP-coloring/operand-order permutation; body and addressing are otherwise byte-identical.
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_00103230(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001A06A0(void);
extern int func_0019CF50(void);

extern float D_70003190[];
extern float D_700031A0[];

int func_0019B4C0(unsigned char *a0, float *a1, float *a2, int flags) {
    float mat[4];
    float tmp[4];
    int i;
    int mode;

    mode = 0;
    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a1[i] - a2[i];
        t = a1[i];
        D_70003190[4 + i] = t;
        tmp[i] = t;
    }
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    func_001028D0(mat, D_700031A0, D_70003190);
    func_00102760(mat, mat);
    func_00103230(mat, mat, 0.01f);
    func_001028D0(D_70003190, D_70003190, mat);

    *(short *)0x7000324E = a0[2] & 0x1F;

    if (flags & 2) {
        *(int *)0x70003254 = *(int *)(a0 + 0x14);
        if (func_001A06A0() == 0) {
            mode = 2;
        }
    }
    if (flags & 4) {
        if (func_0019CF50() == 0) {
            mode = 4;
        }
    }
    func_001028B8(D_70003190, D_70003190, mat);
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[4 + i] = tmp[i];
            D_70003190[12 + i] = D_70003190[8 + i] - D_70003190[4 + i];
        }
        if (flags & 0x80000000) {
            float *p = D_70003190;
            for (i = 0; i < 3; i++) {
                float f = *(float *)(a0 + 0xB0);
                *(float *)(a0 + 0xB0) = p[12] + f;
                p++;
                a0 += 4;
            }
        }
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = mode;
    return mode;
}
