// NEARMISS func_001FA790  (vram 0x001FA790, 0x328 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER permutation. Body and control flow fully recovered and logically identical. mwcc assigns the three saved registers differently from the target: target colors flag-ptr=s0, arg1=s1, arg0=s2 while my compile gets flag-ptr=s2, arg1=s0, arg0=s1, which relabels nearly every s-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 89.5 (mwcc 2.3.3; pinned 991202 = 68.46). Body and control flow
// fully recovered; logic verified against the splat disassembly. The residual
// is a register-allocation-ORDER permutation: the target colors the saved regs
// flag-ptr=s0 / arg1=s1 / arg0=s2, whereas mwcc here picks flag-ptr=s2 /
// arg1=s0 / arg0=s1, relabeling almost every s-register use (cascade of
// ARG_MISMATCH). Also the arg1!=0 guard is a plain branch here vs the target's
// branch-likely with a dual-exit epilogue. All compiler-artifact, not logic.
//
// Lane/clip activation routine. Indexed by arg0 (lane id) into the active-flag
// byte table D_00282154; returns immediately if the lane is already active or
// arg1 (clip id) is 0. Records arg1 into D_00282178[arg0]. For arg0!=0 it reads
// clip params from D_0025E170[arg1] and seeds the per-lane (0x60-stride) state
// arrays at D_00281Fxx/D_002820xx, computing a start angle
// 60*(0.0746667*(field8>>11)) - 30 via func_00128250; for arg0==0 it uses the
// D_0025DD30 table, records the active clip in D_00275B2C, and halves the angle
// term. Then fills the timing/flag fields (1,2,1,1 markers), copies fields, and
// computes a wrap value (>>11 of either half-of-field2 or field8, signed vs
// logical by a magnitude test), marks the lane active, and kicks func_001FABF0.

extern int func_00128250(int, float);
extern void func_001FABF0(int, int, int, int);
extern int D_0025DD30;
extern int D_0025E170;
extern int D_00275B2C;
extern unsigned char D_00281FD0;
extern unsigned char D_00281FD1;
extern unsigned char D_00281FD2;
extern unsigned char D_00281FD3;
extern int D_00281FE4;
extern int D_00281FE8;
extern int D_00281FF0;
extern int D_00281FF4;
extern int D_00281FF8;
extern int D_00281FFC;
extern int D_00282000;
extern int D_00282004;
extern int D_00282008;
extern int D_0028201C;
extern int D_00282020;
extern char D_00282154;
extern int D_00282178;
extern int D_00282188;
extern int D_0028218C;
extern int D_00810E90;

void func_001FA790(int arg0, int arg1) {
    char *flag;
    int *src;
    int base;
    int v;
    unsigned int half;
    unsigned int f8;

    flag = &D_00282154 + arg0;
    if (*flag != 0) {
        return;
    }
    if (arg1 != 0) {
        *(int *)(&D_00282178 + arg0 * 4) = arg1;
        if (arg0 != 0) {
            src = (int *)(&D_0025E170 + arg1 * 0x10);
            base = arg0 * 0x60;
            *(int *)(&D_00281FFC + base) = D_0028218C + src[0];
            *(int *)(&D_00282020 + base) = D_00810E90;
            v = func_00128250(src[0], 60.0f * (0.074666664f * (float)(src[2] >> 11)) - 30.0f);
            *(int *)(&D_0028201C + base) = v;
        } else {
            src = (int *)(&D_0025DD30 + arg1 * 0x10);
            D_00275B2C = arg1;
            base = arg0 * 0x60;
            *(int *)(&D_00281FFC + base) = D_00282188 + src[0];
            *(int *)(&D_00282020 + base) = D_00810E90;
            v = func_00128250(src[0], 60.0f * (0.074666664f * (float)(src[2] >> 11) / 2.0f) - 30.0f);
            *(int *)(&D_0028201C + base) = v;
        }
        base = arg0 * 0x60;
        *(int *)(&D_00281FF0 + base) = src[3];
        *(int *)(&D_00281FF4 + base) = *(int *)(&D_00281FFC + base);
        *(int *)(&D_00281FF8 + base) = (int)(((unsigned int)src[2] + 0x7FF) >> 11) + *(int *)(&D_00281FFC + base);
        *(unsigned char *)(&D_00281FD0 + base) = 1;
        *(unsigned char *)(&D_00281FD2 + base) = 1;
        *(unsigned char *)(&D_00281FD1 + base) = 2;
        *(unsigned char *)(&D_00281FD3 + base) = 1;
        *(int *)(&D_00282008 + base) = *(int *)(&D_00281FE4 + base);
        *(int *)(&D_00282000 + base) = *(int *)(&D_00281FFC + base);
        half = (unsigned int)*(int *)(&D_00281FE8 + base) >> 1;
        f8 = (unsigned int)src[2];
        if (half < f8) {
            *(int *)(&D_00282004 + base) = (int)((unsigned int)(half + 0x7FF) >> 11);
        } else {
            *(int *)(&D_00282004 + base) = (int)(f8 + 0x7FF) >> 11;
        }
        *flag = 1;
        func_001FABF0(arg0, arg1, 0, 0);
    }
}
