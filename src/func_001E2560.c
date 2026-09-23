// NEARMISS func_001E2560  (vram 0x001E2560, 0x294 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Frame 0xA0 in target vs 0x80 here (missing/undersized stack object), two nop branch slots, one add.s and one addu operand order. func_001AFC10 arity corrected (inert).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Turret/emplacement AI state machine dispatched on the state byte at
// arg0+4. arg0+0x24 is a pointer to the linked target/player object; arg0+
// 0x1F0 (aliased "s0") is a small per-instance sub-block used as a
// countdown timer (+0x0 int) and an aim/charge-blend pair (+0x54,+0x5C
// floats). State 0 (init): seeds the timer to func_00122BB8()%40+0x3C,
// clears fields +0xC/+9, advances to state 1, and force-advances to state 3
// (disabled) if func_001E23A0(arg0) or bit 3 of func_001B0070() fire.
// State 1 (active): watches the target's flag byte +2 (low 5 bits) and
// either its downed-float +0x220 or its counter byte +4 (>=2 disables to
// state 3), then only while the target's byte +1 is set runs a nested
// sub-state switch on arg0+5: sub-state 0 counts the timer down and, on
// expiry, advances to sub-state 1 and re-seeds the aim-blend pair via
// func_00122BB8(); sub-state 1 ramps the blend float +0x54 by 0.02f per
// tick (clamping/re-arming the timer + resetting to sub-state 0 past 1.5f),
// then fires a shot: builds a look-at matrix from the linked target's
// per-slot transform (func_001026A0/func_001029C0/func_00102C58/
// func_00102918), resolves a spawn handle (func_001CCF70), and launches a
// projectile via func_001CFA60 + func_001CFBE0(handle, 1, &D_00253670,
// &block, 0). States 2/3 both just forward to func_001AFC10(arg0).
//
// Residuals measured by the m1-firstlevel-matching lane (98.65% on mwcc233):
// (1) the target frame is 0xA0 bytes and this C's is 0x80, so a stack object
// is missing or undersized; (2) two conditional-branch delay slots that the
// target leaves as nop; (3) one add.s and one addu operand order. func_001AFC10
// takes one argument (its byte-matched definition). Removing the extra state
// argument left the object unchanged.
extern int func_001026A0(char *dst, int src, char *m);
extern void func_00102918(char *dst, char *src, char *m);
extern void func_001029C0(char *m);
extern void func_00102C58(char *dst, char *src, char *v);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern int func_001B0070(void);
extern int func_001CCF70(char *a0);
extern void func_001CFA60(void *obj, char *src, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int a4);
extern int func_001E23A0(char *p);
extern int D_00253670;

void func_001E2560(char *arg0) {
    char *s0;
    char *s1;
    unsigned char st;
    unsigned char st2;
    int sp40[16];
    int handle;

    s1 = *(char **)(arg0 + 0x24);
    st = *(unsigned char *)(arg0 + 4);
    s0 = arg0 + 0x1F0;
    switch (st) {
    case 0:
        *(int *)s0 = func_00122BB8() % 40 + 0x3C;
        *(char *)(arg0 + 0xC) = 0;
        *(char *)(arg0 + 9) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        if (func_001E23A0(arg0) != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        if (func_001B0070() & 8) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        return;
    case 1:
        if (!(*(unsigned char *)(s1 + 2) & 0x1F)) {
            if (*(float *)(s1 + 0x220) <= 0.0f) {
                *(unsigned char *)(arg0 + 4) = 3;
                return;
            }
        } else if ((int)*(unsigned char *)(s1 + 4) >= 2) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        if (*(unsigned char *)(s1 + 1) != 0) {
            st2 = *(unsigned char *)(arg0 + 5);
            switch (st2) {
            case 0:
                *(int *)s0 = *(int *)s0 - 1;
                if (*(int *)s0 < 0) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(float *)(s0 + 0x54) = 0.0f;
                    *(float *)(s0 + 0x5C) = (float)func_00122BB8() / 2.1474836e9f;
                    return;
                }
                break;
            case 1:
                *(float *)(s0 + 0x54) = *(float *)(s0 + 0x54) + 0.02f;
                if (!(*(float *)(s0 + 0x54) <= 1.5f)) {
                    *(float *)(s0 + 0x54) = 1.5f;
                    *(int *)s0 = func_00122BB8() % 40 + 0x3C;
                    *(unsigned char *)(arg0 + 5) = 0;
                }
                func_001026A0(arg0 + 0xB0, *(int *)(*(int *)(arg0 + 0x28) * 4 + (int)s1 + 0x110) + 0x90, arg0 + 0xA0);
                func_001029C0(arg0 + 0xD0);
                func_00102C58(arg0 + 0xD0, arg0 + 0xD0, s1 + 0xC0);
                func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
                handle = func_001CCF70(arg0 + 0xB0);
                func_001CFA60(sp40, arg0 + 0xD0, *(float *)(s0 + 0x54), *(float *)(s0 + 0x5C));
                func_001CFBE0(handle, 1, &D_00253670, sp40, 0);
                return;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
