// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00826C50 (splat/link name 00826C10; overlay code is
// linked 0x40 below where it runs), 0x3D8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0 (after func_001B0FD0): state 1, +0 = +8 = 1, +0x30 by +3
//  (0x82C240 / 0x82C260 / 0x82C280), +0x28 = 0, func_001C6380, +0x2E4..2EC =
//  self matrix * (0, 15, 1.35). State 1: func_001B17A0; bit 2 of +0xB starts
//  one of three scripts by +3 (0x82BC40 / 0x82BE40 / 0x82C040), state 4, +0x28
//  = 120. State 4 counts +0x28 down and at 0 sets +0x2A = 1 on two chain
//  members (+0x18 links, depth by +3) with sound 0x19A; at script end state 1,
//  +0xB = 0. States 1/4 run +0x4C and func_001F4BF0 at the stored point
//  (colour 0, 0x80, 0, 0x80). State 3/other func_001AFC10.
extern float D_700038A0[];
extern int D_700038B0[];
extern char D_overlay_AREA16_0082C240[];
extern char D_overlay_AREA16_0082C260[];
extern char D_overlay_AREA16_0082C280[];
extern char D_overlay_AREA16_0082BC40[];
extern char D_overlay_AREA16_0082BE40[];
extern char D_overlay_AREA16_0082C040[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *col);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00826C10(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            self[4] = 1;
            self[0] = 1;
            self[8] = 1;
            switch (self[3]) {
            case 0:
                *(char **)(self + 0x30) = D_overlay_AREA16_0082C240;
                break;
            case 1:
                *(char **)(self + 0x30) = D_overlay_AREA16_0082C260;
                break;
            case 2:
                *(char **)(self + 0x30) = D_overlay_AREA16_0082C280;
                break;
            }
            *(short *)(self + 0x28) = 0;
            func_001C6380(self);
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 15.0f;
            D_700038A0[2] = 1.35f;
            D_700038A0[3] = 1.0f;
            func_001026A0(D_700038A0, self + 0xD0, D_700038A0);
            *(float *)(self + 0x2EC) = D_700038A0[0];
            *(float *)(self + 0x2E8) = D_700038A0[1];
            *(float *)(self + 0x2E4) = D_700038A0[2];
        }
        break;
    case 1:
        func_001B17A0(self);
        if (self[0xB] & 4) {
            switch (self[3]) {
            case 0:
                func_001BA1A0(blk, D_overlay_AREA16_0082BC40);
                break;
            case 1:
                func_001BA1A0(blk, D_overlay_AREA16_0082BE40);
                break;
            case 2:
                func_001BA1A0(blk, D_overlay_AREA16_0082C040);
                break;
            }
            self[4] = 4;
            func_001BA1F0(self);
            *(short *)(self + 0x28) = 120;
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        D_700038A0[0] = *(float *)(self + 0x2EC);
        D_700038A0[1] = *(float *)(self + 0x2E8);
        D_700038A0[2] = *(float *)(self + 0x2E4);
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 0;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0;
        D_700038B0[3] = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 4:
        if (*(short *)(self + 0x28) != 0) {
            (*(short *)(self + 0x28))--;
            if (*(short *)(self + 0x28) <= 0) {
                switch (self[3]) {
                case 0:
                    *(short *)(*(unsigned char **)(self + 0x18) + 0x2A) = 1;
                    *(short *)(*(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18) + 0x2A) = 1;
                    break;
                case 1:
                    *(short *)(*(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18) + 0x2A) = 1;
                    *(short *)(*(unsigned char **)(*(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18) + 0x18) + 0x2A) = 1;
                    break;
                case 2:
                    *(short *)(*(unsigned char **)(*(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18) + 0x18) + 0x2A) = 1;
                    *(short *)(*(unsigned char **)(*(unsigned char **)(*(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18) + 0x18) + 0x18) + 0x2A) = 1;
                    break;
                }
                *(short *)(self + 0x28) = 0;
                func_001FBD50(self, 0x19A, 0, 300.0f);
            }
        }
        if (func_001BA1F0(self) != 0) {
            self[4] = 1;
            self[0xB] = 0;
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        D_700038A0[0] = *(float *)(self + 0x2EC);
        D_700038A0[1] = *(float *)(self + 0x2E8);
        D_700038A0[2] = *(float *)(self + 0x2E4);
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 0;
        D_700038B0[1] = 0x80;
        D_700038B0[2] = 0;
        D_700038B0[3] = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
