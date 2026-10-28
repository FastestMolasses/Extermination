// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.09% NEARMISS).
// Fixes read from the original instructions:
//  - The state dispatch is one switch on +0x04 (3 and any unknown state run
//    func_001AFC10); cases 1 and 2 end in `break`, and the func_001B0FD0
//    early-out of case 0 is a `break`. With `return` mwcc dropped the
//    unreachable slot-fill leftovers the original keeps.
//  - func_001BA1A0's first argument is the sub-object at +0x1F0 (the original
//    computes it once up front), not the item itself.
//  - func_001C5570 is called (item, &D_700038A0, 0x73, 1): the parameter
//    block is the second argument, as in its definition.
//  - +0x30 receives the ADDRESS of D_00275880 / D_00275878 (gp-relative,
//    -sdatathreshold 8); the other globals are absolute (unsized externs).
//  - The second height test compares against floor + 12 (the 6.0f offset is
//    added again), not the same floor + 6 bound twice.
//  - On take, the light-rig object at +0x2EC gets +0x04 = 3 (not 4); the
//    item's own +0x02 is set to 4.

// Item armed-handler pump (func_0015AE20-family contract). State 0 (INIT):
// bind the model (func_001B0FD0 / func_001B1020), patch +0x2E from 3 to 0x14
// when the D_00810C64 flag is set, pick the descriptor, and create the light
// rig (scratchpad parameter block {0, 1.0, 0, 0.25}, id 0x73) at +0x2EC.
// State 1: sub-state +0x05 0 waits for flag 4 in +0x0B and starts the grab
// clip (D_002667E0, or D_00266620 with D_002666B4 = 0x42/0x41/0x40 by height);
// sub-state 1 waits for the clip, fires event 0x194 and finishes. States 1
// and 2 then run func_001B17A0 and the virtual at +0x4C.

extern int func_001B0FD0(int a0);
extern int func_001B1020(char *actor, int model, int clip, int frame);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *actor, float *mtx);
extern char *func_001C5570(char *arg0, int arg1, char arg2, int arg3);
extern int func_001BA1F0(char *actor);
extern void func_001BA1A0(int *a0, int a1);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_001B1190(int id);
extern unsigned char func_001B17A0(char *self);
extern void func_001AFC10(unsigned char *handle);

extern unsigned char D_00810C64[];
extern unsigned char D_00810700[];
extern int D_00275878;
extern int D_00275880;
extern int D_700038A0[4];
extern unsigned char D_008104A0[];
extern unsigned char D_008104E6[];
extern float D_00810354[];
extern int D_002666B4[];
extern int D_002667E0[64];
extern int D_00266620[64];

void func_00219550(unsigned char *self) {
    unsigned char st = self[4];
    int *sub = (int *)(self + 0x1F0);
    float h;

    switch (st) {
    case 0:
        if (self[3] == 0 && *(unsigned short *)(self + 0x2E) == 0x28) {
            if (func_001B0FD0((int)self)) {
                break;
            }
        } else {
            func_001B1020((char *)self, self[0xD], -1, 0);
        }
        if (self[3] == 0) {
            if (*(unsigned short *)(self + 0x2E) == 3) {
                if (D_00810C64[*(unsigned short *)(self + 0x2E)]) {
                    *(unsigned short *)(self + 0x2E) = 0x14;
                }
            }
        }
        func_001C6380(self);
        self[0] = 1;
        self[8] = 3;
        if (D_00810700[0] == 0x10) {
            *(int *)(self + 0x30) = (int)&D_00275880;
        } else {
            *(int *)(self + 0x30) = (int)&D_00275878;
        }
        func_001A2370(self, (float *)(self + 0xD0));
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0x3F800000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3E800000;
        *(char **)(self + 0x2EC) = func_001C5570((char *)self, (int)D_700038A0, 0x73, 1);
        return;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                self[2] = 0x87;
                self[5]++;
                if (D_008104A0[0] == 0x2D || D_008104E6[0] != 0) {
                    func_001BA1A0(sub, (int)D_002667E0);
                } else {
                    h = D_00810354[0] + 6.0f;
                    if (*(float *)(self + 0xB4) < h) {
                        D_002666B4[0] = 0x42;
                    } else if (*(float *)(self + 0xB4) < (h = h + 6.0f)) {
                        D_002666B4[0] = 0x41;
                    } else {
                        D_002666B4[0] = 0x40;
                    }
                    func_001BA1A0(sub, (int)D_00266620);
                }
            }
            break;
        case 1:
            if (func_001BA1F0((char *)self)) {
                func_001FBD50(self, 0x194, 0, 300.0f);
                func_001B1190(self[0x9A]);
                self[4] = 3;
                if (*(unsigned char **)(self + 0x2EC) != 0) {
                    (*(unsigned char **)(self + 0x2EC))[4] = 3;
                }
                self[2] = 4;
            }
            break;
        }
        if (func_001B17A0((char *)self)) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
        if (func_001B17A0((char *)self)) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
