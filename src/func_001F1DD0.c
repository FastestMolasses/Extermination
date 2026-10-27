// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003A24

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.69% NEARMISS).
// Fixes read from the original instructions:
//  - The phase word 0x70003A24 is a relocated scratchpad extern (idiom-32,
//    opted in with // SPAD:), so mwcc may hoist the D_00275B40 table load above
//    the phase store; with a literal address it could not.
//  - The table entry is a typed record, not raw int arithmetic: ten 16-byte
//    float slots (one per frame, frame wraps at 10) followed by ten int flags at
//    +0xA0. Indexing pos[frame][1] and flag[frame] gives the original's
//    address-sum operand order.
//
// Per-frame animation/oscillation tick. arg0+0xD selects a damping constant written
// to scratchpad 0x70003A20 (0.6 for state 1, 0.4 for state 0). arg1 is a {row,frame}
// counter pair: frame (arg1+4) advances 0..9 wrapping, and on wrap row (arg1+0)
// advances 0..5 wrapping. 0x70003A24 gets a random phase in [-pi, pi] from
// func_00122BB8()/2^31 scaled by 2*pi. base = D_00275B40[row]; the current frame slot
// base->pos[frame] is initialized (func_00102948), then nudged in x/y by
// damping * sin/cos(phase) (func_0011E2A8 / func_0011DE90), and base->flag[frame] cleared.
extern void func_00102948(void *, int);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_00122BB8(void);
typedef struct {
    float pos[10][4];
    int flag[10];
} Burst;
extern int D_00275B40;
extern float D_70003A24[4];

void func_001F1DD0(char *arg0, char *arg1, int arg2) {
    Burst *base;
    int st;
    float t;

    st = *(unsigned char *)(arg0 + 0xD);
    switch (st) {
    case 0:
        *(float *)0x70003A20 = 0.4f;
        break;
    case 1:
        *(float *)0x70003A20 = 0.6f;
        break;
    }

    *(int *)(arg1 + 4) = *(int *)(arg1 + 4) + 1;
    if (*(int *)(arg1 + 4) >= 0xA) {
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 0) = *(int *)(arg1 + 0) + 1;
        if (*(int *)(arg1 + 0) >= 6) {
            *(int *)(arg1 + 0) = 0;
        }
    }

    D_70003A24[0] = (6.2831855f * ((float) func_00122BB8() / 2147483648.0f)) - 3.1415927f;

    base = *(Burst **)(D_00275B40 + (*(int *)(arg1 + 0) * 4));
    func_00102948(base->pos[*(int *)(arg1 + 4)], arg2);

    t = *(float *)0x70003A20 * func_0011E2A8(D_70003A24[0]);
    base->pos[*(int *)(arg1 + 4)][0] += t;

    t = *(float *)0x70003A20 * func_0011DE90(D_70003A24[0]);
    base->pos[*(int *)(arg1 + 4)][1] += t;

    base->flag[*(int *)(arg1 + 4)] = 0;
}
