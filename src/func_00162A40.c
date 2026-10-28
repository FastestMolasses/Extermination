// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.38% NEARMISS).
// Fixes read from the original instructions:
//  - every switch case ends in `break` (not `return`); the function returns after the
//    switch. This restores the unfilled compare-chain delay slots and the shared epilogue.
//  - the arg0[0x23F] guard is `> 1` on the unsigned byte (no int cast, not `>= 2`), which
//    puts the compare result in $at.
//  - the 0xB4 float update is a compound `+= -0.2f` (operand order of the add).
//
// State-machine tick: dispatches on the sub-state byte at +6 (cases 0-3 and 10-12),
// copying position fields, starting animations, testing flags at +0x200 and moving the
// object down by 0.2 per tick while falling.

extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern int func_0017F320(int self);
extern int func_001760C0(void *self, void *pos, int flag, float threshold);
extern void func_00174AC0(unsigned char *p, int n);
extern void func_0017C440(unsigned char *e, int f);
extern int func_00188550(unsigned char *e);
extern void func_001764E0(char *p);
extern void func_0017C540(char *p);
extern void func_001796C0(char *p);
extern int func_00175900(unsigned char *e, int flag);
extern void func_00178B90(char *p, int a);
extern void anim_eval_skeleton(char *p);
extern int func_00102948(void *dst, void *src);
extern int func_001FBD50(unsigned char *e, int b, int c, float f);
extern int D_00275B40;

void func_00162A40(unsigned char *arg0) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2E8);
        *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
        if (arg0[0x1F1] == 1) {
            arg0[6] = arg0[6] + 1;
            *(short *)(arg0 + 0x2E) = 0;
            func_001749A0(arg0, 0x7A, 0, 4.0f);
            break;
        }
        arg0[6] = 0xA;
        arg0[0x25F] = 0;
        func_001749A0(arg0, 0x7D, 0, 8.0f);
        func_001764E0((char *)arg0);
        break;
    case 1:
        if (func_0017F320((int)arg0) != 0) {
            arg0[6] = 3;
            break;
        }
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = arg0[6] + 1;
            func_001FBD50(arg0, 0xFF, 0, 300.0f);
            break;
        }
        break;
    case 2:
        if (func_0017F320((int)arg0) != 0) {
            arg0[6] = 3;
            break;
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 9;
            arg0[6] = 0;
            arg0[0x1F0] = 0x10;
            arg0[0xD] = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            break;
        }
        break;
    case 3:
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        arg0[5] = 7;
        arg0[6] = 0;
        arg0[0x1F0] = 0xD;
        *(float *)(arg0 + 0x2EC) = -0.20000000298023224f;
        break;
    case 10:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            anim_eval_skeleton((char *)arg0);
            func_00102948(arg0 + 0xB0, (void *)(*(int *)(D_00275B40 + 4) + 0xC0));
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 11.0f;
            func_001749A0(arg0, 0x8C, 0, 0.0f);
            if (func_001760C0(arg0, arg0 + 0xB0, 1, 18.0f) != 0) {
                arg0[0x236] = 1;
                arg0[0x235] = arg0[0x235] | 2;
            }
            arg0[6] = arg0[6] + 1;
            break;
        }
        break;
    case 11:
        func_00174AC0(arg0, 0);
        if (arg0[0x23F] > 1) {
            arg0[6] = arg0[6] + 1;
            func_0017C440(arg0, 1);
        } else {
            arg0[0x25C] = 0;
            func_0017C540((char *)arg0);
            func_001764E0((char *)arg0);
        }
        *(float *)(arg0 + 0xB4) += -0.2f;
        if (func_00175900(arg0, 1) == 0) {
            *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
            arg0[5] = 7;
            arg0[6] = 0;
            arg0[0x1F0] = 0xD;
            break;
        }
        break;
    case 12:
        func_00178B90((char *)arg0, 1);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540((char *)arg0);
        }
        *(float *)(arg0 + 0xB4) += -0.2f;
        func_00175900(arg0, 1);
        func_001796C0((char *)arg0);
        break;
    }
}
