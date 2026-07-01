// NEARMISS func_0021F330  (vram 0x0021F330, 0x520 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring: at the arg0+0x224 and arg0+0x22C `!= 0.0f` zero-compares, target colors field into $f0 / zero into $f1, mwcc 2.3.3 colors them the other way (same c.eq.s, swapped operands). Tried reversing source comparison operand order (no effect, compiler normalizes) and explicit `float ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state driver on state byte at arg0+6 (linear if-else chain of beq's against
// 0xD,0xC,0xB,0xA,3,2,1,0; unhandled values 4-9 fall straight through to the tail).
// Tail (always run): func_001764E0(arg0); if arg0+0xF!=0 set D_008106BC=1; then
// func_00179880(arg0, arg0+0x2EC); func_00175900(arg0,1).
//
// state 0: bump state, zero the attack-flag byte arg0+7, fire func_001749A0(0x2C,..)
//  (clip index passed via a raw pointer-as-int arg0, matching the target's odd call),
//  func_00174AC0(arg0,0), refresh the blend timer arg0+0x26C from arg0+0x24C, zero
//  arg0+0x28 / arg0+0x2FE / arg0+0x2EC.
// state 1 (the big one): countdown timer arg0+0x2FE; on underflow reset it + arg0+0x28;
//  func_00174AC0(arg0,0); if a hit-test or the debug byte at 0x70003B8D fires, bump
//  state, OR in flag 4 on arg0+0, play hit react clips, zero arg0+0x224, done.
//  Else if global gate D_0081083C==0, reset to a base combat state (arg0+0=3, timer
//  arg0+0x20E=0x3C, clear 0x25C, func_0017C540). Else run the fade-out (0x224) / fade-in
//  (0x22C) sub-logic, dispatch a death-direction pick (arg0+4/5/0x1F0) when the knockback
//  height (0x220) has settled, else re-arm the stagger blend rate at arg0+0x204 based on
//  the attack flag arg0+7 and the combo counter arg0+0x28.
// state 2: bump to state 3 once flag 0x8000 in arg0+0x200 clears, and set D_008106BC=1.
// state 3: on flag 0x1000 in arg0+0x200, reset to base combat state and zero 0x224/0x22C.
// state 10: bump state, fire death scream clip 0x1C7.
// state 11: once arg0+0x3C (a fall/lie timer) exceeds 60.0f, bump state and call
//  func_0021C120 (ragdoll/corpse settle).
// state 12: poll func_0021C190 for "settled"; on settle bump state, else re-arm the
//  blend rate 0x204 to a fixed rest value.
// state 13: same base-combat-state reset as state 3, or rearm 0x204 default.
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_00174AC0(unsigned char *e, int f);
extern int func_001754E0(unsigned char *e, int f);
extern int func_00175900(unsigned char *e, int f);
extern void func_001764E0(unsigned char *e);
extern void func_00179880(unsigned char *e, void *p);
extern void func_0017C540(unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *p, int a, int b, float f);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021C270(unsigned char *e);
extern void func_0021C350(unsigned char *e);
extern unsigned char D_008106BC;
extern unsigned char D_008106F1;
extern unsigned char D_0081083C;

void func_0021F330(unsigned char *arg0) {
    unsigned char st;
    signed char c;
    float fb;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x2C, 0, 0.0f);
        func_00174AC0(arg0, 0);
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        *(short *)(arg0 + 0x28) = 0;
        *(signed char *)(arg0 + 0x2FE) = 0;
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        c = *(signed char *)(arg0 + 0x2FE);
        *(signed char *)(arg0 + 0x2FE) = c - 1;
        if (c == 0) {
            *(signed char *)(arg0 + 0x2FE) = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        func_00174AC0(arg0, 0);
        if (func_001754E0(arg0, 6) != 0 || *(unsigned char *)0x70003B8D != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned char *)(arg0 + 0) = *(unsigned char *)(arg0 + 0) | 4;
            func_001749A0(arg0, 0x2D, 0, 8.0f);
            func_001FBD50(arg0, 0x15A, 0, 300.0f);
            *(int *)(arg0 + 0x224) = 0;
            goto state1_done;
        }
        if (D_0081083C == 0) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            goto state1_done;
        }
        if (*(float *)(arg0 + 0x224) != 0.0f) {
            func_0021C350(arg0);
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                *(unsigned char *)(arg0 + 0) = 2;
                if (*(unsigned char *)(arg0 + 0x234) == 1) {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 3;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = 0x3F;
                } else {
                    *(signed char *)(arg0 + 4) = 2;
                    *(signed char *)(arg0 + 5) = 1;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(signed char *)(arg0 + 0x1F0) = 0x40;
                }
                return;
            }
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(arg0, 0x154, 0, 300.0f);
            func_001749A0(arg0, 0x1E, 1, 1.0f);
            *(unsigned char *)(arg0 + 7) = 1;
        } else if ((fb = *(float *)(arg0 + 0x22C)) != 0.0f) {
            func_0021C270(arg0);
            if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1 != 0) {
                *(unsigned char *)(arg0 + 6) = 0xA;
                D_008106BC = 1;
            } else {
                func_001B61C0(0, 0xC0, 5, 1);
                func_001FBD50(arg0, 0x154, 0, 300.0f);
                func_001749A0(arg0, 0x1E, 1, 1.0f);
                *(unsigned char *)(arg0 + 7) = 1;
            }
        }
        switch (*(unsigned char *)(arg0 + 7)) {
        case 0:
            if (*(short *)(arg0 + 0x28) != 0) {
                *(int *)(arg0 + 0x204) = 0x400CCCCD;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F000000;
            }
            break;
        case 1:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_001749A0(arg0, 0x2C, 0, 4.0f);
                *(unsigned char *)(arg0 + 7) = 0;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F99999A;
            }
            break;
        }
    state1_done:
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        break;
    case 2:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            D_008106BC = 1;
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            *(int *)(arg0 + 0x224) = 0;
            *(int *)(arg0 + 0x22C) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x1C7, 0, 1.0f);
        break;
    case 11:
        if (*(float *)(arg0 + 0x3C) <= 60.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021C120(arg0);
        }
        break;
    case 12:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        } else {
            *(int *)(arg0 + 0x204) = 0x3DCCCCCD;
        }
        break;
    case 13:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            D_008106BC = 0;
            *(unsigned char *)(arg0 + 0) = 3;
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(signed char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
            *(int *)(arg0 + 0x224) = 0;
            *(int *)(arg0 + 0x22C) = 0;
        } else {
            *(int *)(arg0 + 0x204) = 0x3F000000;
        }
        break;
    }
    func_001764E0(arg0);
    if (*(unsigned char *)(arg0 + 0xF) != 0) {
        D_008106BC = 1;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
