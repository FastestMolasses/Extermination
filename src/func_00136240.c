// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Two-state actor update (self=arg0, work=arg1) dispatched on the state byte at
// arg0+6. State 0: if both the halfword at +0x34 is 0 and (+0x36 & 0xA000)==0,
// just bump the sub-timer byte at +5 and stop; otherwise advance state, set
// work+0x20 = 1.0f, compute self+0xC4 = func_001B1240(self+0xB0, D_00810360,
// D_00810368), zero work+0x2C/+0x30, then per the +0x36 0x8000 flag either play
// clip 0x1B (work+0x2C = -1.0f, event 0x856) or clip 0x1C (event 0x854 + low
// bit of func_00122BB8) at 450.0f. Shared tail (state 1 lands here): while the
// clip id at +0x2C is exactly 0x1B and self+0x3C <= 130.0f, nudge work+0x2C/+0x30
// by the sign of work+0x2C (set +0x30 = 0.03f when <=0). Finally, if work+0
// has bit 0x1000, either (timer +0x34 set) reset the actor and fire event 0x84C
// when func_00122BB8 bits clear, or (else) re-arm byte +5 = 1.
//
// Matched with mwcc 2.3.3 (991202 reaches 89.6%). Three keys: (1) the state
// bump must be `*(u8*)(arg0+6) += 1` (NOT `= *(u8*)(arg0+6) + 1`) so mwcc
// re-loads the byte like the target instead of CSE'ing the switch dispatch
// value into a speculative branch-likely; (2) the event float is 450.0f
// (0x43E10000), not 451.0f; (3) the `c != 0.0f` test needs an explicit
// `float zero = 0.0f;` local so mwcc colors c into $f1 and zero into $f0,
// matching the target's c.eq.s/c.le.s operand order. objdiff 100.0 vs expected.
extern float func_001B1240(char *p, float a, float b);
extern int func_00122BB8(void);
extern void anim_clip_init(char *p, int clip, float speed, float d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern float D_00810360;
extern float D_00810368;

void func_00136240(char *arg0, char *arg1) {
    short v;

    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        if (*(short *)(arg0 + 0x34) == 0 &&
            (*(short *)(arg0 + 0x36) & 0xA000) == 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            break;
        }
        *(unsigned char *)(arg0 + 6) += 1;
        *(float *)(arg1 + 0x20) = 1.0f;
        *(float *)(arg0 + 0xC4) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        *(int *)(arg1 + 0x30) = 0;
        *(int *)(arg1 + 0x2C) = 0;
        if (*(short *)(arg0 + 0x36) & 0x8000) {
            *(float *)(arg1 + 0x2C) = -1.0f;
            anim_clip_init(arg0, 0x1B, 5.0f, 0.0f);
            func_001FBD50(arg0, 0x856, 0, 450.0f);
        } else {
            anim_clip_init(arg0, 0x1C, 5.0f, 0.0f);
            func_001FBD50(arg0, (func_00122BB8() & 1) + 0x854, 0, 450.0f);
        }
        /* fallthrough into the shared tail (case 1 also lands here) */
    case 1:
        v = *(short *)(arg0 + 0x2C);
        if ((v & 0x8000) == 0 && v == 0x1B &&
            *(float *)(arg0 + 0x3C) <= 130.0f) {
            float c = *(float *)(arg1 + 0x2C);
            float zero = 0.0f;
            if (c != zero) {
                if (c > zero) {
                    *(int *)(arg1 + 0x2C) = 0;
                    *(int *)(arg1 + 0x30) = 0;
                } else {
                    *(int *)(arg1 + 0x30) = 0x3CF5C28F;
                }
            }
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(short *)(arg0 + 0x34) != 0) {
                *(unsigned char *)(arg0 + 4) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 0) = 1;
                *(unsigned char *)(arg1 + 0x5D) = 0;
                *(short *)(arg0 + 0x36) = 0;
                *(short *)(arg1 + 0x52) = 0x78;
                *(short *)(arg1 + 0x50) = 0;
                *(short *)(arg1 + 0x56) = 0x12C;
                *(int *)(arg1 + 0x2C) = 0;
                if (((func_00122BB8() >> 17) & 0xF) == 0) {
                    func_001FBD50(arg0, 0x84C, 0, 450.0f);
                }
            } else {
                *(unsigned char *)(arg0 + 5) = 1;
                *(unsigned char *)(arg0 + 6) = 0;
                *(int *)(arg1 + 0x30) = 0;
                *(int *)(arg1 + 0x2C) = 0;
            }
        }
        break;
    }
}
