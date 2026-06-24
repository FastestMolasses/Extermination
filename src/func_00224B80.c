// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor state-machine step, dispatched on the state byte at e+7 (states
// 0,1,2,0xA,0xB,0x14..0x18,0x1E). Returns 2 on the death/teleport transitions
// (states 0xB,0x1E), 0 when state 0 finds both velocity floats (+0x224,+0x22C)
// zero, and 1 otherwise (shared tail).
//  * state 0: if vel +0x224 != 0 -> play clip 0x152 (func_001FBD50, 300.0f),
//    step physics (func_0021C350); pick next state by the +0x220 sign, the
//    +0xF==0x63 sentinel and the +0x234==1 flag (0x1E / 0xA), else advance and
//    arm timer +0x20E=0x3C. Else if vel +0x22C != 0 -> clip 0x153, func_0021C270,
//    next state 0x14 or advance gated on +0x228>=100 && D_008106F1, timer 0x3C.
//  * states 0xA/0xB use func_001B61C0 (big/small shake) + clip plays; 0xB also
//    func_0021D490 then returns 2. 0x1E arms bytes 4/5/6/0x1F0 and returns 2.
//  * func_001749A0(self, clip, flags, blend) takes its blend float in $f12 (so
//    self stays in $a0); func_0021C120/C190/D490 all take the actor pointer.
//
// C-shape notes for the match (mwcc 2.3.3, mwcps2-2.3.3-000906; the pinned
// 991202 caps at 84.9% on the clean-store delay-slot-nop family, wall #13):
//  * `float z = 0.0f` + `field != z` (NOT `!= 0.0f`) flips c.eq.s to the f0,f1
//    operand order of the target.
//  * cases that do work then return 1 must `break` to the single trailing
//    `return 1;` (shared exit), not return per-arm, or mwcc duplicates `li v0,1`.
//  * the +0xF==0x63 / +0x234==1 paths share the `e+7 = 0x1E` store via goto, as
//    the original did; the +0x22C branch must store e+7 inside each if/else arm
//    (NOT a shared post-if store) to keep the sb in the branch delay slot.
// Verified objdiff 100.0% vs build/expected/func_00224B80.o (278/278 insns).
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_0021C350(unsigned char *);
extern void func_0021C270(unsigned char *);
extern void func_0021C120(unsigned char *);
extern int func_0021C190(unsigned char *);
extern void func_0021D490(unsigned char *);
extern int func_00122BB8(void);
extern void func_001749A0(unsigned char *, int, int, float);
extern unsigned char D_008106F1;

int func_00224B80(unsigned char *e) {
    unsigned char state;
    float z = 0.0f;

    state = *(unsigned char *)(e + 7);
    switch (state) {
    case 0:
        if (*(float *)(e + 0x224) != z) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(e, 0x152, 0, 300.0f);
            func_0021C350(e);
            if (*(float *)(e + 0x220) <= 0.0f) {
                if (*(unsigned char *)(e + 0xF) == 0x63) {
                    goto set_1E;
                }
                if (*(unsigned char *)(e + 0x234) == 1) {
set_1E:
                    *(unsigned char *)(e + 7) = 0x1E;
                } else {
                    *(unsigned char *)(e + 7) = 0xA;
                }
            } else {
                *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
                *(short *)(e + 0x20E) = 0x3C;
            }
            break;
        }
        if (*(float *)(e + 0x22C) != z) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(e, 0x153, 0, 300.0f);
            func_0021C270(e);
            if (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1 != 0) {
                *(unsigned char *)(e + 7) = 0x14;
            } else {
                *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
            }
            *(short *)(e + 0x20E) = 0x3C;
            break;
        }
        return 0;
    case 1:
        *(unsigned char *)(e + 7) = state + 1;
        if (*(unsigned char *)(e + 0xF) & 2) {
            func_001749A0(e, 0x67, 0, 4.0f);
            *(unsigned char *)(e + 0xF) = 0;
        } else if (func_00122BB8() & 1) {
            func_001749A0(e, 0x67, 0, 4.0f);
        } else {
            func_001749A0(e, 0x66, 0, 4.0f);
        }
        break;
    case 2:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 7) = 0;
            func_001749A0(e, 0x5F, 0, 8.0f);
            *(short *)(e + 0x20E) = 0x3C;
        }
        break;
    case 0xA:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(e, 0x146, 0, 300.0f);
        func_001FBD50(e, 0x151, 0, 300.0f);
        *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
        func_001749A0(e, 0x68, 0, 8.0f);
        *(char *)(e + 0x1F0) = 0x40;
        break;
    case 0xB:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 6) = 0x1E;
            *(unsigned char *)(e + 7) = 0;
            func_0021D490(e);
            func_001B61C0(1, 0xEE, 0x3C, 1);
            return 2;
        }
        break;
    case 0x14:
        *(unsigned char *)(e + 7) = state + 1;
        func_001749A0(e, 0x67, 0, 4.0f);
        break;
    case 0x15:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 7) = state + 1;
        }
        break;
    case 0x16:
        if (*(float *)(e + 0x3C) <= 32.0f) {
            *(unsigned char *)(e + 7) = state + 1;
            func_0021C120(e);
        }
        break;
    case 0x17:
        if (func_0021C190(e) != 0) {
            *(unsigned char *)(e + 7) = *(unsigned char *)(e + 7) + 1;
        }
        break;
    case 0x18:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 7) = 0;
            func_001749A0(e, 0x5F, 0, 8.0f);
            *(short *)(e + 0x20E) = 0x3C;
        }
        break;
    case 0x1E:
        *(char *)(e + 4) = 2;
        *(char *)(e + 5) = 3;
        *(char *)(e + 6) = 0;
        *(char *)(e + 0x1F0) = 0x3F;
        return 2;
    }
    return 1;
}
