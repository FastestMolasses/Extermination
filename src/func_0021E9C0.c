// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Entity per-state tick, dispatched on the state byte at e+6. State 0 (spawn):
// play sfx func_001FBD50(e, 0x154, 0, 300.0f), request rumble
// func_001B61C0(0, 0xC0, 5, 1), advance the state byte and clear the substate
// byte +7, kick animation clip 0x20 via func_001749A0(e, 0x20, 0, 1.0f), then
// zero the motion accumulators +0x38 / +0x21C / +0x2EC. State 1: if flag bit
// 0x1000 of +0x200 is set, clear +0xF, arm the 0x20E timer to 0x3C, clear
// +0x25C and call func_0017C540(e, 1); otherwise integrate the vertical delta
// from (*D_00275B40)[2] into +0x38, latch it into +0x21C, and call
// func_00178B90(e, 1). Any other state falls straight through. All paths end
// with func_00179880(e, e + 0x2EC); func_00175900(e, 1).
extern void func_001FBD50(char *e, int a, int b, float f);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001749A0(char *e, int a, int b, float f);
extern void func_00179880(char *e, char *p);
extern void func_00175900(char *e, int a);
extern void func_0017C540(char *e, int a);
extern void func_00178B90(char *e, int a);
extern char **D_00275B40;

void func_0021E9C0(char *e) {
    switch (*(unsigned char *)(e + 6)) {
    case 0:
        func_001FBD50(e, 0x154, 0, 300.0f);
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        *(char *)(e + 7) = 0;
        func_001749A0(e, 0x20, 0, 1.0f);
        *(int *)(e + 0x38) = 0;
        *(int *)(e + 0x21C) = 0;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 0xF) = 0;
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e, 1);
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        break;
    }
    func_00179880(e, e + 0x2EC);
    func_00175900(e, 1);
}
