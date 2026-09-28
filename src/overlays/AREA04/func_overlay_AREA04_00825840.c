// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA04 overlay, runtime 0x00825880 (splat/link name 00825840;
// overlay code is linked 0x40 below where it runs), 0x27C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 func_001B0FD0 == 0 -> state 1, +0 = 1, +0x30 =
// &D_00275928, func_001C6380. State 1 func_001B17A0; +0xB bit 2 starts
// script 0x82C1F0, state 4, pumps it, link +0x28 = 120, +0x28 = 0. State 4
// counts +0x28 up to 120 (func_001FBD50(self, 0x19A, 0, 300) on reaching
// it) and returns to state 1 at script end. States 1 and 4 draw
// func_001F4BF0 at (+0xB0, +0xB4 - 0.04, +0xB8 + 0.34) with color (0,
// 0x80, 0, 0x80) and run the +0x4C method. Other states
// func_001AFC10.
extern int D_00275928;
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA04_0082C1F0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001F4BF0(float *pos, int *color);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00825840(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            self[4] = 1;
            self[0] = 1;
            *(int **)(self + 0x30) = &D_00275928;
            func_001C6380(self);
        }
        break;
    case 1:
        func_001B17A0(self);
        if (self[0xB] & 4) {
            func_001BA1A0(blk, D_overlay_AREA04_0082C1F0);
            self[4] = 4;
            func_001BA1F0(self);
            *(short *)(*(char **)(self + 0x18) + 0x28) = 0x78;
            *(short *)(self + 0x28) = 0;
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        D_700038A0[0] = *(float *)(self + 0xB0);
        D_700038A0[1] = *(float *)(self + 0xB4) - 0.04f;
        D_700038A0[2] = 0.34f + *(float *)(self + 0xB8);
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 0;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0;
        D_700038B0[3] = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 4:
        if (*(short *)(self + 0x28) < 0x78) {
            *(short *)(self + 0x28) += 1;
            if (*(short *)(self + 0x28) == 0x78) {
                func_001FBD50(self, 0x19A, 0, 300.0f);
            }
        }
        if (func_001BA1F0(self) != 0) {
            self[4] = 1;
            self[0xB] = 0;
        }
        D_700038A0[0] = *(float *)(self + 0xB0);
        D_700038A0[1] = *(float *)(self + 0xB4) - 0.04f;
        D_700038A0[2] = 0.34f + *(float *)(self + 0xB8);
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 0;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0;
        D_700038B0[3] = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
