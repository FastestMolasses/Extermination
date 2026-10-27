// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.07% NEARMISS).
// Fixes read from the original instructions:
// - The bone-loop index and the anim_advance_time result are ONE variable,
//   zeroed at function entry (the original keeps it in one saved register and
//   reads the zero in state 1 when the timer has not expired). Two separate
//   variables produced the "s0<->s1 coloring wall".
// - Every switch case ends in `break` (falling to the shared return), not
//   `return`; this restores the original's unfilled callback-check slots and
//   the call argument in the indirect call's delay slot.
// - The pose-matrix rows are copied through a uint128 pointer to D_00275B40's
//   rows (q[0..3]), which fixes the a1/a2 register choice of the copy.
//
// A 4-state actor-animation update: 0 = allocate bones + first-frame init,
// 1 = advance the clip timer and rebuild the transform from the shared pose
// D_00275B40, dispatching a user callback (tail+0xFC) on either a state-flag
// reject or a low-scale guard; 2/3 = free via func_001AFC10.
typedef unsigned __int128 uint128;
extern int func_001AF780(void);
extern void func_001AFC10(unsigned char *actor);
extern void func_001C68C0(unsigned char *o);
extern void anim_bone_array_setup(unsigned char count);
extern void bone_init_default_2(unsigned char *o, short a1);
extern int anim_advance_time(int self, float t);
extern unsigned char **D_00275B40;
extern short D_00275BCC;

void func_001C7EB0(unsigned char *arg0) {
    int i = 0; /* loop index in state 0, anim_advance_time result in state 1 */
    unsigned char *tail;
    unsigned char *cur;
    unsigned char *rec;
    unsigned char st;
    int flags;
    void (*cb)(unsigned char *);
    unsigned char *src;
    uint128 *q;

    rec = *(unsigned char **)(arg0 + 0x24);
    tail = arg0 + 0x1F0;
    st = arg0[4];

    switch (st) {
    case 0:
        arg0[0xC] = 1;
        if (D_00275BCC < arg0[0xC]) {
            arg0[4] = 3;
            break;
        }
        cur = arg0;
        while (i < arg0[0xC]) {
            *(int *)(cur + 0x110) = func_001AF780();
            cur += 4;
            i++;
        }
        arg0[9] = arg0[0xC];
        anim_bone_array_setup(arg0[0xC]);
        cb = *(void (**)(unsigned char *))(tail + 0xFC);
        if (cb != 0) {
            cb(arg0);
        }
        bone_init_default_2(arg0, *(short *)(arg0 + 0x94));
        *(int *)(tail + 0xF4) = 1;
        arg0[4] = 1;
        break;

    case 1:
        if (rec != 0 && (*(unsigned short *)(rec + 0x2E) & (1 << *(unsigned short *)(arg0 + 0x2E)))) {
            arg0[4] = 3;
            break;
        }
        *(int *)(tail + 0xF4) = *(int *)(tail + 0xF4) - 1;
        if (*(int *)(tail + 0xF4) < 0) {
            i = anim_advance_time((int)arg0, *(float *)(tail + 0xF8));
        }
        *(int *)(arg0 + 0xB0) = 0;
        *(int *)(arg0 + 0xB4) = 0;
        *(int *)(arg0 + 0xB8) = 0;
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        *(int *)(arg0 + 0xC0) = 0;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg0 + 0xC8) = 0;
        *(int *)(arg0 + 0xCC) = 0x3F800000;
        func_001C68C0(arg0);
        *(float *)(arg0 + 0xB0) = *(float *)(*D_00275B40 + 0xC0);
        *(float *)(arg0 + 0xB4) = *(float *)(*D_00275B40 + 0xC4);
        *(float *)(arg0 + 0xB8) = *(float *)(*D_00275B40 + 0xC8);
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        q = (uint128 *)(*D_00275B40 + 0x90);
        *(uint128 *)(arg0 + 0xD0) = q[0];
        *(uint128 *)(arg0 + 0xE0) = q[1];
        *(uint128 *)(arg0 + 0xF0) = q[2];
        *(uint128 *)(arg0 + 0x100) = q[3];

        flags = i & 0x3000;
        *(int *)(arg0 + 0x10C) = 0x3F800000;
        if (flags) {
            arg0[4] = 3;
            cb = *(void (**)(unsigned char *))(tail + 0xFC);
            if (cb != 0) {
                cb(arg0);
                break;
            }
        } else {
            src = *D_00275B40;
            if (*(float *)(src + 0x18) < 0.8999999761581421f &&
                *(float *)(src + 0x1C) < 0.8999999761581421f &&
                *(float *)(src + 0x20) < 0.8999999761581421f) {
                break;
            }
            cb = *(void (**)(unsigned char *))(tail + 0xFC);
            if (cb != 0) {
                cb(arg0);
                break;
            }
        }
        break;

    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
