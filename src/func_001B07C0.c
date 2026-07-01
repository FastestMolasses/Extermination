// NEARMISS func_001B07C0  (vram 0x001B07C0, 0x390 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// D_00275BE0 is addressed absolute (lui+lbu) under -sdatathreshold 0 here, but the target addresses it gp-relative (lbu v1,0($gp)) at both of its two reads in this function. This is the same documented, unresolved gp-rel-vs-absolute wall already recorded on sibling func_001B0460.c (shares the same ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-frame HUD/camera-target record builder, sibling of func_001B0250/func_001B0300/
// func_001B0460 (same D_0024D650[area][slot] + D_00810702*0x30 record lookup). Builds the record
// at D_008102B0 (aliased here as `base`): when D_00275BE0==1 (a "fixed/cutscene camera" flag) both
// the "from" (0xA0..0xAC) and "to" (0xB0..0xBC) transform slots are seeded from the fixed globals
// D_00810710/14/18 with an extra rotation/scale block (0xC0..0xC8) copied from D_00810720/24/28;
// otherwise both slots are seeded from the live record `p` (p+0/4/8), with the rotation block
// zeroed. All 8 vectors (0xB0..0xCC) are then mirrored out to the fixed hardware/DMA staging
// addresses 0x70003B40-0x7000B35C. D_00810706 (a 1-bit "active" flag, masked &1) is written back
// and mirrored to base+0x235/0x234 together with D_00810707. Wind/ambient floats D_00810858/5C are
// copied to base+0x220/0x228. If D_008106C8 bit 2 is set, a tri-state D_00810C60 selector is
// derived from D_00810C7D/7E and, if bits 0x60 are also set, func_001EFE00(0x80000018, base) is
// invoked and its result stashed at base+0x304 (an owned sub-object pointer); if bit 2 is clear,
// D_00810C60 is reset to 0 and any existing base+0x304 owner is torn down (its +4 byte set to 2,
// pointer cleared) before func_0015C1F0(base) runs the per-mode HUD-kind dispatch. base+0xE is
// then set from the record's +0x14 byte and eight orientation/scale floats (0x60/64/68/6C/80/84/
// 88/8C) reset to 1.0f, base+0x230 (a state/command byte) cleared. Finally, when D_00275BE0==1 the
// +0xE byte is forced back to 0 (cutscene camera never targets); otherwise, if arg0 != 0: a
// pending +0xE==1 "cut" request re-arms itself (4=5,5=1,6=0), a live +0x1C listener is notified
// (+4=1) unless the hard gate *0x70003B8D is set, and any nonzero pending pan offset (+0x224/
// +0x22C) is consumed and flagged via +0x0=1. func_001B0460(arg0) runs last (unconditionally).

extern void func_001B0250(void *a, int b);
extern int func_001EFE00(int a, void *b);
extern void func_0015C1F0(void *a);
extern void func_001B0460(int a0);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00275BE0;
extern void *D_0024D650[];
extern void *D_008102B0;
extern float D_00810710;
extern float D_00810714;
extern float D_00810718;
extern float D_00810720;
extern float D_00810724;
extern float D_00810728;
extern volatile unsigned char D_00810706;
extern unsigned char D_00810707;
extern float D_00810858;
extern float D_0081085C;
extern int D_008106C8;
extern unsigned char D_00810C7D;
extern unsigned char D_00810C7E;
extern unsigned char D_00810C60;

void func_001B07C0(int arg0) {
    unsigned char *base;
    unsigned char *p;
    unsigned char flags;

    base = (unsigned char *)&D_008102B0;
    p = (unsigned char *)((void **)D_0024D650[D_00810700])[D_00810701];
    p += D_00810702 * 0x30;

    func_001B0250((void *)D_0024D650[D_00810700], D_00810701 * 4);

    if (D_00275BE0 == 1) {
        *(float *)(base + 0xA0) = D_00810710;
        *(float *)(base + 0xA4) = D_00810714;
        *(float *)(base + 0xA8) = D_00810718;
        *(int *)(base + 0xAC) = 0x3F800000;
        *(float *)(base + 0xB0) = D_00810710;
        *(float *)(base + 0xB4) = D_00810714;
        *(float *)(base + 0xB8) = D_00810718;
        *(int *)(base + 0xBC) = 0x3F800000;
        *(float *)(base + 0xC0) = D_00810720;
        *(float *)(base + 0xC4) = D_00810724;
        *(float *)(base + 0xC8) = D_00810728;
        *(int *)(base + 0xCC) = 0x3F800000;
    } else {
        *(float *)(base + 0xA0) = *(float *)(p + 0);
        *(float *)(base + 0xA4) = *(float *)(p + 4);
        *(float *)(base + 0xA8) = *(float *)(p + 8);
        *(int *)(base + 0xAC) = 0x3F800000;
        *(float *)(base + 0xB0) = *(float *)(p + 0);
        *(float *)(base + 0xB4) = *(float *)(p + 4);
        *(float *)(base + 0xB8) = *(float *)(p + 8);
        *(int *)(base + 0xBC) = 0x3F800000;
        *(int *)(base + 0xC0) = 0;
        *(float *)(base + 0xC4) = *(float *)(p + 0xC);
        *(int *)(base + 0xC8) = 0;
        *(int *)(base + 0xCC) = 0x3F800000;
    }

    flags = D_00810706;
    *(volatile float *)0x70003B40 = *(float *)(base + 0xB0);
    *(volatile float *)0x70003B44 = *(float *)(base + 0xB4);
    *(volatile float *)0x70003B48 = *(float *)(base + 0xB8);
    flags &= 1;
    *(volatile float *)0x70003B4C = *(float *)(base + 0xBC);
    *(volatile float *)0x70003B50 = *(float *)(base + 0xC0);
    *(volatile float *)0x70003B54 = *(float *)(base + 0xC4);
    *(volatile float *)0x70003B58 = *(float *)(base + 0xC8);
    *(volatile float *)0x70003B5C = *(float *)(base + 0xCC);

    D_00810706 = flags;
    *(base + 0x235) = D_00810706;
    *(base + 0x234) = D_00810707;
    *(float *)(base + 0x220) = D_00810858;
    *(float *)(base + 0x228) = D_0081085C;

    if (D_008106C8 & 4) {
        if (D_00810C7E != 0) {
            if (D_00810C7D != 0) {
                D_00810C60 = 2;
            } else {
                D_00810C60 = 1;
            }
        } else {
            D_00810C60 = 0;
        }
        if (D_008106C8 & 0x60) {
            *(int *)(base + 0x304) = func_001EFE00(0x80000018, base);
        }
    } else {
        D_00810C60 = 0;
        if (*(int *)(base + 0x304) != 0) {
            *(*(unsigned char **)(base + 0x304) + 4) = 2;
            *(int *)(base + 0x304) = 0;
        }
    }

    func_0015C1F0(base);

    *(base + 0xE) = *(p + 0x14);
    *(int *)(base + 0x60) = 0x3F800000;
    *(int *)(base + 0x64) = 0x3F800000;
    *(int *)(base + 0x68) = 0x3F800000;
    *(int *)(base + 0x6C) = 0x3F800000;
    *(int *)(base + 0x80) = 0x3F800000;
    *(int *)(base + 0x84) = 0x3F800000;
    *(int *)(base + 0x88) = 0x3F800000;
    *(int *)(base + 0x8C) = 0x3F800000;
    *(int *)(base + 0x230) = 0;

    if (D_00275BE0 == 1) {
        *(base + 0xE) = 0;
    } else if (arg0 != 0) {
        if (*(base + 0xE) == 1) {
            *(base + 4) = 5;
            *(base + 5) = 1;
            *(base + 6) = 0;
        }
        if (*(int *)(base + 0x1C) != 0 && *(volatile unsigned char *)0x70003B8D == 0) {
            *(*(unsigned char **)(base + 0x1C) + 4) = 1;
        }
        {
            float zero = 0.0f;
            if (*(float *)(base + 0x224) != zero || *(float *)(base + 0x22C) != zero) {
                *(int *)(base + 0x224) = 0;
                *(int *)(base + 0x22C) = 0;
                *(base + 0) = 1;
            }
        }
    }

    func_001B0460(arg0);
}
