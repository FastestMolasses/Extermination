// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00827B60 (splat/link name 00827B20; overlay code
//  is linked 0x40 below where it runs), 0x264 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 1 placements (x11: [6]..[8], [20], [26], [28]..[33]): state 3
//  when D_0081079E == 0xFF; when func_001B0FD0 returns 0: +0x34 = 1, +0 = 1
//  and the pose copied to +0x200 / +0x210. State 1: +5 0 waits for D_0081081E
//  == 1 and loads the countdown +0x28 = +0x9A; at zero +0 = 2, state 2, and a
//  func_0019A570 probe from 4 above to 4 below the position; on a hit
//  func_001F0460(4, ..) at the matrix turned by pi/2 and lifted by 0.2. Then
//  func_001B17A0 and the +0x4C method. State 2: +0x10 = D_00156620.
typedef void (*ActorFn)(unsigned char *);
extern char D_00156620[];
extern unsigned char D_0081079E;
extern unsigned char D_0081081E;
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001C6380(unsigned char *self);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102918(void *a, void *b, void *c);
extern void func_001F0460(int kind, void *m);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00827B20(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (D_0081079E == 0xFF) {
            self[4] = 3;
            break;
        }
        if (func_001B0FD0(self) == 0) {
            *(short *)(self + 0x34) = 1;
            self[0] = 1;
            func_00102948(blk + 0x10, self + 0xB0);
            func_00102948(blk + 0x20, self + 0xC0);
            func_001C6380(self);
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081081E == 1) {
                self[5]++;
                *(short *)(self + 0x28) = self[0x9A];
            }
            break;
        case 1:
            if (--*(short *)(self + 0x28) == 0) {
                self[0] = 2;
                self[4] = 2;
                self[5] = 0;
                func_00102948(D_700038A0, self + 0xB0);
                func_00102948(D_700038B0, D_700038A0);
                D_700038A0[1] += 4.0f;
                D_700038B0[1] -= 4.0f;
                if (func_0019A570(D_700038A0, D_700038B0, 4, 0) != 0) {
                    func_001029C0(D_700036A0);
                    func_00102C58(D_700036A0, D_700036A0, self + 0xC0);
                    func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                    func_00102918(D_700036A0, D_700036A0, self + 0xB0);
                    *(float *)0x700036D4 += 0.2f;
                    func_001F0460(4, D_700036A0);
                }
            }
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
        *(void **)(self + 0x10) = D_00156620;
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
