// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.31% NEARMISS).
// Fixes read from the original instructions:
//  - Every case ends in `break` (not `return`), and case 0's two clip calls are
//    an if/else: this gives the branch-likely entry to case 0 with the duplicated
//    increment, the unfilled compare-chain slots and the duplicated constant setup.
//  - Case 3 passes arg0 + 0xD0 directly instead of through a reassigned local
//    pointer, so the angle store follows the wrap call as in the original.
//  - func_00182430 takes (entity, gait) — two arguments, as its definition in src/.
//
// Entity state machine on arg0[6]: 0 starts a clip (0x94 for kind 0x1B, else
// 0x95); 1 waits for flag 0x8000 at +0x200 to clear; 2 steps sub-state arg0[7]
// as the float at +0x3C drops to per-kind thresholds, calling the step helpers and
// finally func_001FBD50 with id 0x119; 3 waits for flag 0x1000, adds pi to the
// angle at +0xC4 (kind 0x1C only), rebuilds the matrix at +0xD0 from a fixed
// scratchpad offset and the object held at D_00275B40+4, then sets state 0xE,
// sub 0, kind 0x1D.

extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00182AB0(void *obj);
extern void func_00182430(unsigned char *e, unsigned char gait);
extern void func_001FBD50(void *p, int id, int flags, float vol);
extern float func_001B1470(float a);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *dst, int a, void *c);
extern void func_00102918(void *dst, void *a, void *b);
extern void func_0017FF80(void *p, float a);
extern char *D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00167C80(unsigned char *arg0) {
    unsigned char st;
    unsigned char sub;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[7] = 0;
        if (arg0[0x1F0] == 0x1B) {
            func_001749A0(arg0, 0x94, 0, 8.0f);
        } else {
            func_001749A0(arg0, 0x95, 0, 8.0f);
        }
        break;
    case 1:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        break;
    case 2:
        if (arg0[0x1F0] == 0x1B) {
            sub = arg0[7];
            switch (sub) {
            case 0:
                if (*(float *)(arg0 + 0x3C) <= 24.0f) {
                    arg0[7] = sub + 1;
                    func_00182AB0(arg0);
                }
                break;
            case 1:
                if (*(float *)(arg0 + 0x3C) <= 5.0f) {
                    arg0[6] = st + 1;
                    func_00182AB0(arg0);
                    func_001FBD50(arg0, 0x119, 0, 300.0f);
                }
                break;
            }
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            if (*(float *)(arg0 + 0x3C) <= 99.0f) {
                arg0[7] = sub + 1;
                func_00182430(arg0, 2);
            }
            break;
        case 1:
            if (*(float *)(arg0 + 0x3C) <= 69.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            break;
        case 2:
            if (*(float *)(arg0 + 0x3C) <= 48.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            break;
        case 3:
            if (*(float *)(arg0 + 0x3C) <= 23.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            break;
        case 4:
            if (*(float *)(arg0 + 0x3C) <= 2.0f) {
                arg0[6] = st + 1;
                func_00182AB0(arg0);
                func_001FBD50(arg0, 0x119, 0, 300.0f);
            }
            break;
        }
        break;
    case 3:
        if (!(*(int *)(arg0 + 0x200) & 0x1000)) {
            break;
        }
        if (arg0[0x1F0] == 0x1C) {
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
        }
        func_001029C0(arg0 + 0xD0);
        func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0xC1266666;
        *(int *)0x700038A8 = 0x4089999A;
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, *(int *)(D_00275B40 + 4) + 0xC0, D_700038B0);
        *(float *)(arg0 + 0xBC) = 1.0f;
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
        arg0[0x2F1] = 0;
        func_0017FF80(arg0, 0.0f);
        arg0[5] = 0xE;
        arg0[6] = 0;
        arg0[0x1F0] = 0x1D;
        break;
    }
}
