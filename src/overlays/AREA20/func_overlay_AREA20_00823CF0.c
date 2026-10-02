// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823D30 (splat/link name 00823CF0; overlay code
//  is linked 0x40 below where it runs), 0x710 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: sub 0 placements [19]..[31]: breakable objects. State 0: after
//  func_001B0FD0, +0x34 = 1, +0 = 1, the spawn position / rotation saved in
//  +0x200 / +0x210, func_001C6380. State 1: a shot (+0x36) gives state 2 and,
//  when func_0019A570 hits along +-4 in y, effect 4 (func_001F0460) on the
//  +0xC0 / pi/2 frame; func_001B1B70 and the +0x4C method unless D_70003B92
//  is set and D_0081080C == 2. State 2, +5: 0 effects 0x80000013 and
//  0x8000001C (kinds 0x18 / 0x2A, sound 0x1A1) or 0x8000002E (sound 0x19F) at
//  +0xB0 + (0, 7, 0), +0x28 = 2, a random heading (kind 10: 2pi * rand & 0xF0
//  / 256; kind 12: +0xC4 + pi * (rand & 0x1F) / 180), speed and lift from
//  tables 0x827E70 / 0x827E80; 1 after 2 frames +0xC4 = heading, kinds 0x18 /
//  0x2A end (state 3); 2 the fall: yaw turn (0.017453292 rising, 0.008726646
//  falling), movement, func_0019AD00 wall stop, gravity -0.06 down to -4;
//  landing (func_0019AB20) effects 0x80000013 / 0x8000002E, sound 0x1A0,
//  state 3; else every 64th frame func_001B0D80. Then func_001C6380,
//  func_001A2370, func_001B17A0 and the +0x4C method. State 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
#define F(p, o) (*(float *)((p) + (o)))
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern unsigned char D_70003B92[];
extern int D_70003B68[];
extern short D_70003B8A[];
extern unsigned char D_0081080C;
extern float D_overlay_AREA20_00827E70[];
extern float D_overlay_AREA20_00827E80[];
extern int func_001B0FD0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001C6380(unsigned char *self);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, void *v);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102918(void *dst, void *src, void *v);
extern void func_001F0460(int id, void *m);
extern void func_001B1B70(unsigned char *self);
extern void func_001EFD20(int id, void *pos);
extern void func_001FC580(unsigned char *self, int id);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern int func_0019AD00(unsigned char *self, void *pos, int flags);
extern int func_0019AB20(unsigned char *self, void *pos, void *dir, int flags);
extern void func_001B0D80(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA20_00823CF0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        S16(0x34) = 1;
        self[0] = 1;
        func_00102948(blk + 0x10, self + 0xB0);
        func_00102948(blk + 0x20, self + 0xC0);
        func_001C6380(self);
        break;
    case 1:
        if (S16(0x36) != 0) {
            self[0] = 2;
            self[4] = 2;
            func_00102948(D_700038A0, self + 0xB0);
            func_00102948(D_700038B0, D_700038A0);
            D_700038A0[1] += 4.0f;
            D_700038B0[1] -= 4.0f;
            if (func_0019A570(D_700038A0, D_700038B0, 4, 0) != 0) {
                func_001029C0(D_700036A0);
                func_00102C58(D_700036A0, D_700036A0, self + 0xC0);
                func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                func_00102918(D_700036A0, D_700036A0, self + 0xB0);
                D_700036A0[13] += 0.2f;
                func_001F0460(4, D_700036A0);
            }
        }
        if (D_70003B92[0] == 0 || D_0081080C != 2) {
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
        switch (self[5]) {
        case 0:
            D_700038A0[0] = F(self, 0xB0);
            D_700038A0[1] = 7.0f + F(self, 0xB4);
            D_700038A0[2] = F(self, 0xB8);
            D_700038A0[3] = 1.0f;
            func_001EFD20(0x80000013, D_700038A0);
            if (self[3] == 0x18 || self[3] == 0x2A) {
                func_001EFD20(0x8000001C, D_700038A0);
                func_001FC580(self, 0x1A1);
            } else {
                func_001EFD20(0x8000002E, D_700038A0);
                func_001FC580(self, 0x19F);
            }
            S16(0x28) = 2;
            self[5] = self[5] + 1;
            if (self[3] == 10) {
                F(blk, 0x74) = func_001B1470(6.2831855f * (float)(func_00122BB8() & 0xF0) / 256.0f);
            } else if (self[3] == 12) {
                float a = (float)(func_00122BB8() & 0x1F);
                F(blk, 0x74) = func_001B1470(F(self, 0xC4) + 3.1415927f * a / 180.0f);
            }
            F(self, 0x38) = D_overlay_AREA20_00827E70[(func_00122BB8() & 0x300) >> 8];
            F(blk, 0x78) = D_overlay_AREA20_00827E80[(func_00122BB8() & 0x300) >> 8];
            break;
        case 1:
            if (--S16(0x28) == 0) {
                self[5] = self[5] + 1;
                F(self, 0xC4) = F(blk, 0x74);
                if (self[3] == 0x18 || self[3] == 0x2A) {
                    self[4] = 3;
                    self[5] = 0;
                }
            }
            break;
        case 2:
            if (F(blk, 0x78) < 0.0f) {
                F(self, 0xC0) = func_001B1470(0.017453292f + F(self, 0xC0));
            } else {
                F(self, 0xC0) = func_001B1470(0.008726646f + F(self, 0xC0));
            }
            F(self, 0xB0) += F(self, 0x38) * func_0011E2A8(F(blk, 0x74));
            F(self, 0xB8) += F(self, 0x38) * func_0011DE90(F(blk, 0x74));
            D_700038A0[0] = F(self, 0xB0) + 7.0f * func_0011E2A8(F(blk, 0x74));
            D_700038A0[1] = 7.0f + F(self, 0xB4);
            D_700038A0[2] = F(self, 0xB8) + 7.0f * func_0011DE90(F(blk, 0x74));
            if (F(self, 0x38) != 0.0f && func_0019AD00(self, D_700038A0, 0x80000007) != 0) {
                F(self, 0x38) = 0.0f;
            }
            F(blk, 0x78) -= 0.06f;
            if (F(blk, 0x78) < -4.0f) {
                F(blk, 0x78) = -4.0f;
            }
            F(self, 0xB4) += F(blk, 0x78);
            if (F(blk, 0x78) < 0.0f) {
                D_700038A0[0] = 0.0f;
                D_700038A0[1] = -10.0f;
                D_700038A0[2] = 0.0f;
                D_700038A0[3] = 1.0f;
                if (func_0019AB20(self, self + 0xB0, D_700038A0, 0x80000007) != 0) {
                    D_700038A0[0] = F(self, 0xB0);
                    D_700038A0[1] = 4.0f + F(self, 0xB4);
                    D_700038A0[2] = F(self, 0xB8);
                    D_700038A0[3] = 1.0f;
                    func_001EFD20(0x80000013, D_700038A0);
                    func_001EFD20(0x8000002E, D_700038A0);
                    func_001FC580(self, 0x1A0);
                    S16(0x36) = 0;
                    self[4] = 3;
                    self[5] = 0;
                } else if (((D_70003B68[0] + D_70003B8A[0]) & 0x3F) == 0) {
                    func_001B0D80(self);
                }
            }
            break;
        }
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
