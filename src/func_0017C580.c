// NEARMISS func_0017C580  (vram 0x0017C580, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// One register: the +0x0F value of the 0x63 test is $a0 where the target has $a2 (2 instrs). Tried if/goto, ||, switch, int/uchar temps, pointer temp, duplicated blocks, a C double compare, mwcc 2.4/991202 (also $a0); permuter 25.9k iterations stayed at the base score.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Player landing (+0x05 = 8), called by the fall (func_00162DB0 sub-state
// 0xB) once the footing flag +0x0A is set. It sets +0x05 = 8, +0x06 = 0,
// +0x1F0 = 0xF, runs func_00182870(self, 1) and clears +0x25C and the speed
// +0x38.
//
// The infected-death reaction (+0x05 = 3, the one func_0021C440 picks for
// an infected player) takes over first: when the damage type +0x0F is 0x63,
// or when func_001000E0(func_00128350(health +0x220), 0) is nonzero
// (func_00128350 = float to double) while the infected latch +0x234 is 1,
// it sets +0x00 = 2, +0x04 = 2, +0x05 = 3, +0x06 = 0, +0x1F0 = 0x3F and
// returns.
//
// Otherwise the drop d = height +0xB4 - the fall-start height +0x2F4 is
// published to the scratchpad float 0x70003A20 and picks the sub-state:
//   d < -104       fatal: +0x06 = 1, rumble (big motor on, 0xEE, 60
//                  frames, forced), +0x00 = 2, health = 0, +0x25F = 0, sound
//                  0x151, clip 0x2B, +0x1F0 = 0x40.
//   else           func_00174AC0(self, 0) (stick and gait update), then
//     health <= 0              +0x06 = 3;
//     infection +0x228 >= 100 and D_008106F1 set   +0x06 = 0xA;
//     d <= -50     a gait-3 landing facing the stick (func_001755B0 == 0)
//                  gives +0x06 = 2 and sound 0x13D; any other gives
//                  +0x06 = 3, rumble (small motor 0xD0, 10 frames), 5.0
//                  pending damage applied by func_0021C350, sound 0x151;
//     d <= -14.5   the same gait-3 test gives +0x06 = 2 and sound 0x13D,
//                  else +0x06 = 4;
//     higher       +0x06 = 5.
// Every path except the infected-death return ends with +0x07 = 0. D_008106F1 is declared as an
// array to keep its absolute address; D_70003A20 is the scratchpad float
// as a relocated extern (MATCHING_GUIDE idiom-32).
extern int func_001000E0(int, int);
extern int func_00128350(float);
extern void func_001749A0(char *, int, int, float);
extern void func_00174AC0(char *, int);
extern int func_001755B0(char *);
extern void func_00182870(char *, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(char *, int, int, float);
extern void func_0021C350(char *);

extern unsigned char D_008106F1[16];
extern float D_70003A20[4];

void func_0017C580(char *self) {
    float drop;

    *(char *)(self + 5) = 8;
    *(char *)(self + 6) = 0;
    *(char *)(self + 0x1F0) = 0xF;
    func_00182870(self, 1);
    *(char *)(self + 0x25C) = 0;
    *(int *)(self + 0x38) = 0;
    if (*(unsigned char *)(self + 0xF) == 0x63) {
        goto reset;
    }
    if (func_001000E0(func_00128350(*(float *)(self + 0x220)), 0) != 0 &&
        *(unsigned char *)(self + 0x234) == 1) {
reset:
        *(char *)(self + 0) = 2;
        *(char *)(self + 4) = 2;
        *(char *)(self + 5) = 3;
        *(char *)(self + 6) = 0;
        *(char *)(self + 0x1F0) = 0x3F;
        return;
    }
    drop = *(float *)(self + 0xB4) - *(float *)(self + 0x2F4);
    D_70003A20[0] = drop;
    if (drop < -104.0f) {
        *(char *)(self + 6) = 1;
        func_001B61C0(1, 0xEE, 0x3C, 1);
        *(char *)(self + 0) = 2;
        *(float *)(self + 0x220) = 0.0f;
        *(char *)(self + 0x25F) = 0;
        func_001FBD50(self, 0x151, 0, 300.0f);
        func_001749A0(self, 0x2B, 0, 1.0f);
        *(char *)(self + 0x1F0) = 0x40;
    } else {
        func_00174AC0(self, 0);
        if (*(float *)(self + 0x220) <= 0.0f) {
            *(char *)(self + 6) = 3;
        } else if (*(float *)(self + 0x228) >= 100.0f && D_008106F1[0] != 0) {
            *(char *)(self + 6) = 0xA;
        } else {
            drop = D_70003A20[0];
            if (drop <= -50.0f) {
                if (*(unsigned char *)(self + 0x23F) == 3 && func_001755B0(self) == 0) {
                    *(char *)(self + 6) = 2;
                    func_001FBD50(self, 0x13D, 0, 300.0f);
                } else {
                    *(char *)(self + 6) = 3;
                    func_001B61C0(0, 0xD0, 0xA, 1);
                    *(int *)(self + 0x224) = 0x40A00000;
                    func_0021C350(self);
                    func_001FBD50(self, 0x151, 0, 300.0f);
                }
            } else if (drop <= -14.5f) {
                if (*(unsigned char *)(self + 0x23F) == 3 && func_001755B0(self) == 0) {
                    *(char *)(self + 6) = 2;
                    func_001FBD50(self, 0x13D, 0, 300.0f);
                } else {
                    *(char *)(self + 6) = 4;
                }
            } else {
                *(char *)(self + 6) = 5;
            }
        }
    }
    *(char *)(self + 7) = 0;
}
