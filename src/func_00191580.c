// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003A20, 0x70003A24
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.

// Semantics: writes a smoothed pitch delta to scratchpad 0x70003A20
// (= D_0081069C - func_0011DF78(arg0+0xC)); if below a per-state floor
// (-20 when arg0+0x64 == -46.8f else -10) it damps the excess by 0.5 and
// clamps the stored value at 0x70003A24 to >= -1.5, then calls
// func_0018C5A0(arg0+0x10, 11.0 + composed_height, 4.0).

extern float D_70003A20;
extern float D_70003A24;
extern float func_0011DF78(float);
extern void func_0018C5A0(unsigned char *, float, float);
extern void func_001916C0(unsigned char *, unsigned char *, int);
extern float D_0081069C;

void func_00191580(unsigned char *arg0, unsigned char *arg1) {
    float t;
    float lo;
    float d;
    float v;

    func_001916C0(arg0, arg1, 0);
    D_70003A20 = D_0081069C - func_0011DF78(*(float *)(arg0 + 0xC));
    if (*(float *)(arg0 + 0x64) == -46.8f) {
        lo = -20.0f;
    } else {
        lo = -10.0f;
    }
    t = D_70003A20;
    if (t < lo) {
        d = 0.5f * (t - lo);
        D_70003A24 = d;
        if (d < -1.5f) {
            D_70003A24 = -1.5f;
        }
        v = 11.0f + (*(float *)(arg0 + 0x8C) + (*(float *)(arg1 + 0xA4) + (*(float *)(arg0 + 0x5C) - D_70003A24)));
    } else {
        v = 11.0f + (*(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xA4)));
    }
    func_0018C5A0(arg0 + 0x10, v, 4.0f);
}
