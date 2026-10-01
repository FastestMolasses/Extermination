// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA19 overlay, runtime 0x00825C70 (splat/link name 00825C30; overlay code
//  is linked 0x40 below where it runs), 0x270 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [9]. State 0 when func_001B0FD0 returns 0:
//  func_001C6380, +0 = 1, +0x1F8 = 0.0577; state 2 and func_0019C6F0(0x21, 0)
//  when D_00810776 == 0xFF, else state 1 and func_0019C6F0(0x21, 1). State 1:
//  +5 0 waits for D_00810776 == 1; +5 1 counts +0x28 to 300, then for 260
//  frames (func_001EFD90(2, (1012, 135, 917.6), (0, pi, 0, 1)) at 300) adds
//  +0x1F8 to the +0x80 float of the object at *(D_00275B40 + 4) and twice
//  that to the one at *(D_00275B40 + 8); after that, when D_00810776 == 0xFF,
//  it zeroes both, sets state 2 and func_0019C6F0(0x21, 0). The +0x4C method
//  in states 1 and 2.
typedef void (*ActorFn)(unsigned char *);
extern char *D_00275B40;
extern unsigned char D_00810776[];
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_0019C6F0(int id, int on);
extern void func_001EFD90(unsigned int msg, void *pos, void *dir);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00825C30(unsigned char *self) {
    short t;
    char *b;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
            *(float *)(self + 0x1F8) = 0.057692308f;
            if (D_00810776[0] == 0xFF) {
                self[4] = 2;
                func_0019C6F0(0x21, 0);
            } else {
                self[4] = 1;
                func_0019C6F0(0x21, 1);
            }
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810776[0] == 1) {
                self[5] = 1;
                *(short *)(self + 0x28) = 0;
            }
            break;
        case 1:
            t = *(short *)(self + 0x28);
            if (t < 300) {
                *(short *)(self + 0x28) = t + 1;
            } else if (t < 560) {
                if (t == 300) {
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038A0 = 1012.0f;
                    *(float *)0x700038A4 = 135.0f;
                    *(float *)0x700038A8 = 917.6f;
                    *(float *)0x700038AC = 1.0f;
                    *(float *)0x700038B4 = 3.1415927f;
                    *(float *)0x700038B8 = 0.0f;
                    *(float *)0x700038BC = 1.0f;
                    func_001EFD90(2, D_700038A0, D_700038B0);
                }
                *(short *)(self + 0x28) += 1;
                *(float *)(*(char **)(D_00275B40 + 4) + 0x80) += *(float *)(self + 0x1F8);
                b = *(char **)(D_00275B40 + 8);
                *(float *)(b + 0x80) = *(float *)(b + 0x80) + 2.0f * *(float *)(self + 0x1F8);
                func_001C6380(self);
            } else if (D_00810776[0] == 0xFF) {
                *(float *)(*(char **)(D_00275B40 + 8) + 0x80) = 0.0f;
                *(float *)(*(char **)(D_00275B40 + 4) + 0x80) = 0.0f;
                self[4] = 2;
                func_0019C6F0(0x21, 0);
                func_001C6380(self);
            }
            break;
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
