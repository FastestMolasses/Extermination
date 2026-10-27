// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.22% NEARMISS).
// Fixes read from the original instructions:
//  - func_00184BA0 takes ONE argument (its definition in src/ has one); the $a1=1
//    the original sets beforehand belongs to the following func_00174AC0 call.
//  - The dispatch is a real `switch` on the state byte (case 0 falls into case 1,
//    every case ends in `break`, the failed-call paths `return`), written in the
//    style of the byte-matched sibling func_0016FCF0 (unsigned char *p, locals
//    declared at the top, `+=` for the 0xB4 decay): this restores the unfilled
//    compare-chain delay slots and the duplicated case-0 increment.
//  - The 0x23A sound selector is `m == 6 || m == 5` sharing one call, not two
//    separate arms; the clip id is an int holding the short return value.
//  - The 0x240 speed test is a plain truth test (`if (speed)`), which orders the
//    float compare operands like the original.
//
// Actor state step dispatched on the state byte p[6]:
//  case 0: advance state, clear p[7] and the word at +0x2EC, then run case 1.
//  case 1: unless func_001607D0 or func_00160220 report busy (then return without
//          the shared tail), tick (func_00174AC0/0017BC40/0017C030/00178B90); stage
//          p[0x1F0] 6/7 advances the state and clears the timer at +0x28, stage 0
//          resets p[5]/p[6].
//  case 2: unless func_001607D0 is busy: if the pad mask D_00810E74 hits the
//          scratchpad pad word and func_00184BA0 accepts, call func_001798D0, set
//          p[5]=0x25, p[6]=0 and return. Otherwise tick; outside stages 6/7 a nonzero
//          speed at +0x240 picks clip speed D_00248870[p[0x23F]-1] and starts the clip
//          (func_001749A0 for p[0x1F1]==3, else anim_clip_arbiter at half the
//          func_001C61D0 length), stepping the state back; a zero speed resets.
//          In stages 6/7, every 8th tick of +0x28 plays sound 0x80000033 (selector
//          6 or 5) or 0x80000012 (when p[0x23C] and p[0x23D] are both clear).
//  case 0x63: tick, and func_0017C540 unless flag 0x8000 of +0x200 is set.
// Shared tail: func_001764E0, decay +0xB4 by 0.8 (p[0x23B]==0x35) or 0.4, then
// func_00175900(p,1), func_001756E0, func_001796C0.
extern void anim_clip_arbiter(unsigned char *p, int clip, float speed, float f);
extern int func_00160220(unsigned char *p);
extern int func_001607D0(unsigned char *p);
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_00174AC0(unsigned char *p, int n);
extern void func_001756E0(unsigned char *p);
extern void func_00175900(unsigned char *p, int f);
extern void func_001764E0(unsigned char *p);
extern void func_00178B90(unsigned char *p, int a);
extern void func_001796C0(unsigned char *p);
extern void func_001798D0(unsigned char *p);
extern short func_0017B490(unsigned char *e, int a1, unsigned char variant, unsigned char a3);
extern void func_0017BC40(unsigned char *p);
extern void func_0017C030(unsigned char *p);
extern void func_0017C540(unsigned char *p);
extern int func_00184BA0(unsigned char *p);
extern int func_001C61D0(int a, int b);
extern void func_001EFD90(int id, unsigned char *a, unsigned char *b);

extern float D_00248870[];
extern unsigned short D_00810E74;

void func_001612D0(unsigned char *p) {
    unsigned char state;
    unsigned char sub;
    int clip;

    state = p[6];
    switch (state) {
    case 0:
        p[6] = state + 1;
        p[7] = 0;
        *(int *)(p + 0x2EC) = 0;
    case 1:
        if (func_001607D0(p) != 0) {
            return;
        }
        if (func_00160220(p) != 0) {
            return;
        }
        func_00174AC0(p, 1);
        func_0017BC40(p);
        func_0017C030(p);
        func_00178B90(p, 0);
        sub = p[0x1F0];
        if (sub == 6 || sub == 7) {
            p[6] = p[6] + 1;
            *(short *)(p + 0x28) = 0;
        } else if (sub == 0) {
            p[5] = 0;
            p[6] = 0;
        }
        break;
    case 2:
        if (func_001607D0(p) != 0) {
            return;
        }
        if (D_00810E74 & *(unsigned short *)0x70003B76) {
            if (func_00184BA0(p) != 0) {
                func_001798D0(p);
                p[5] = 0x25;
                p[6] = 0;
                return;
            }
        }
        func_00174AC0(p, 1);
        sub = p[0x1F0];
        if (sub != 6 && sub != 7) {
            if (*(float *)(p + 0x240)) {
                p[0x25C] = p[0x23F] - 1;
                *(float *)(p + 0x38) = D_00248870[p[0x25C]];
                clip = func_0017B490(p, 1, p[0x235], p[0x25C]);
                if (p[0x1F1] == 3) {
                    func_001749A0(p, clip, 0, 0.0f);
                } else {
                    *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(p + 0x40), clip);
                    anim_clip_arbiter(p, clip, 0.0f, *(float *)0x70003A20 / 2.0f);
                }
                p[6] = p[6] - 1;
                p[0x1F0] = 1;
                p[0x1F1] = 1;
            } else {
                p[5] = 0;
                p[6] = 0;
                p[0x1F0] = 0;
            }
        } else {
            if (!(*(short *)(p + 0x28) & 7)) {
                sub = p[0x23A];
                if (sub == 6 || sub == 5) {
                    func_001EFD90(0x80000033, p + 0xB0, p + 0xC0);
                } else if (p[0x23C] == 0 && p[0x23D] == 0) {
                    func_001EFD90(0x80000012, p + 0xB0, p + 0xC0);
                }
            }
            *(short *)(p + 0x28) = *(short *)(p + 0x28) + 1;
        }
        func_0017BC40(p);
        func_0017C030(p);
        func_00178B90(p, 0);
        break;
    case 0x63:
        func_00174AC0(p, 1);
        func_00178B90(p, 0);
        if (!(*(int *)(p + 0x200) & 0x8000)) {
            func_0017C540(p);
        }
        break;
    }
    func_001764E0(p);
    if (p[0x23B] == 0x35) {
        *(float *)(p + 0xB4) += -0.8f;
    } else {
        *(float *)(p + 0xB4) += -0.4f;
    }
    func_00175900(p, 1);
    func_001756E0(p);
    func_001796C0(p);
}
