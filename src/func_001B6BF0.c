// NEARMISS func_001B6BF0  (vram 0x001B6BF0, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Multi-artifact near-miss (93.58% mwcc991): inverse-CSE of constant 8, CW empty branch-delay-slot scheduling (NOT the clean-store nop case, so 2.3.3 does not help -> 233 is 92.44%), FP-coloring of func_0021B9A0 args, store-scheduling of the {0,-1,0,1} block, andi-at-callsite. All permuter/scheduli...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 93.58% (mwcc 991202; 233 = 92.44%). Body/logic fully recovered.
// Multiple independent compiler artifacts remain, all permuter/scheduling
// class:
//  * inverse-CSE: target re-materializes the constant 8 both in the beq
//    delay slot AND before the func_001B0C00 call; mwcc CSEs it (one copy).
//  * branch-delay-slot fill: target leaves nop in the 'b' slots of the
//    return-0 paths where both mwcc builds fill the slot with the return
//    value (paddub v0,zero,zero) -- CW empty-slot scheduling, not the
//    clean-store nop case, so 2.3.3 does not fix it.
//  * FP-coloring of the func_0021B9A0(0,0.0f,0.0f) args (fa0f vs fa1).
//  * store-scheduling: target groups the two zero stores (0x5F0,0x5F8)
//    before the -1.0/1.0 stores; mwcc reorders.
//  * andi-at-callsite: target masks a0 to byte right before func_001B0250;
//    mwcc keeps the byte value live without re-masking.
// First param is passed in $a1 (an unused $a0 precedes it -> int arg0).
//
// Semantics: state-machine step keyed on arg1[4]. case0: if init-substate
// (arg1[0xC]) not 2 -> set 3, return 2; else run func_001B0C00(8), advance
// substate, refresh via func_001B6250, return 0. case1: when D_0028A9A0==2
// (mode-ready), stop/flush (func_001FBC50/FABB0), set D_002821B4=2, advance
// arg1[4], init a status block at D_008102B0 (+0x1F2/0x20C/0x2F3/0x40), and
// if D_008101E4==3 do first-time setup: func_001B0250, func_0021B9A0,
// func_001D2830(2,0), func_001D25F0(480.0f), and write the const quaternion
// {0,-1,0,1} at D_008105F0..FC; set D_008101E4=2; func_001B6250; return 1.

extern void func_001B0250(unsigned char);
extern void func_001B0C00(int);
extern int func_001B6250(void *);
extern void func_001D25F0(float);
extern void func_001D2830(int, int);
extern void func_001FABB0(void);
extern void func_001FBC50(void);
extern void func_0021B9A0(int, float, float);

extern int D_002821B4;
extern int D_0028A580;
extern short D_0028A9A0;
extern unsigned char D_008101E4;
extern unsigned char D_008102B0;
extern int D_008105F0;
extern int D_008105F4;
extern int D_008105F8;
extern int D_008105FC;
extern unsigned char D_00810E40;

int func_001B6BF0(int arg0, unsigned char *arg1) {
    unsigned char *base = &D_008102B0;

    switch (arg1[4]) {
    case 0:
        if (*(char *)(arg1 + 0xC) != 2) {
            *(char *)(arg1 + 0xC) = 3;
            return 2;
        }
        func_001B0C00(8);
        arg1[4] = arg1[4] + 1;
        func_001B6250(&D_00810E40);
        return 0;
    case 1:
        if (D_0028A9A0 == 2) {
            unsigned char v;
            func_001FBC50();
            func_001FABB0();
            D_002821B4 = 2;
            v = arg1[4] + 1;
            arg1[4] = v;
            *(short *)(base + 0x1F2) = 0;
            *(short *)(base + 0x20C) = -1;
            *(char *)(base + 0x2F3) = 3;
            *(int *)(base + 0x40) = D_0028A580;
            if (D_008101E4 == 3) {
                func_001B0250(v);
                func_0021B9A0(0, 0.0f, 0.0f);
                func_001D2830(2, 0);
                func_001D25F0(480.0f);
                D_008105F0 = 0;
                D_008105F8 = 0;
                D_008105F4 = 0xBF800000;
                D_008105FC = 0x3F800000;
            }
            D_008101E4 = 2;
            func_001B6250(&D_00810E40);
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}
