// NEARMISS func_overlay_AREA17_00825520 (99.41%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00825560 (splat/link name 00825520; overlay code
//  is linked 0x40 below where it runs), 0x934 bytes.
// NEARMISS: overlay_match.py check AREA17 scores 99.41% (lane OVLC); the
//  overlay links this function from its splat .s.
// Role: sub 0 placements [5], [6], [7], [8]: breakable objects (the AREA20
//  0x823D30 family). State 0 as there. State 1: a shot (+0x36) breaks it
//  (state 2) and, when func_0019A570 hits along +-4 in y, effect 4
//  (func_001F0460) on the +0xC0 / pi/2 frame; D_00810805 >= 3 also sets state
//  3; the +0x4C method; with func_00102738 (an xyz dot product) of the offset
//  D_00810350 - +0xB0 (w 0) in 0x70003A20 at most 0x70003A24 = 50 * 50, +1 =
//  1 and func_001B1D20, else func_001B17A0. State 2, +5: 0 the AREA20 burst
//  (effects 0x80000013 and 0x8000001C / 0x8000002E, sounds 0x1A1 / 0x19F)
//  with a random heading and speed (tables 0x828E10 / 0x828E20); 1 after 2
//  frames kinds 0x18 / 0x2A end, others wait 8 more; 2 a second wait, then +0
//  = 1, +0x36 = 0, +0x34 = 1; 3 (and 2) the fall: heading / yaw turn,
//  movement, func_0019AD00 wall stop, gravity -0.06 down to -4; a shot (+0x36
//  without bit 0x2000) bursts it again (state 3, +0 = 2); landing
//  (func_0019AB20) the 0x1A0 burst and state 3; else every 64th frame
//  func_001B0D80, and in area 0x15 below y 5 effect 0x8000005F and
//  func_001FBD50(self, 0xDB, 0, 800) and state 3. Then func_001C6380,
//  func_001A2370, func_001B17A0 and the +0x4C method. State 3 func_001AFC10.
// Divergence: in the state-1 range test the original loads 0x70003A20 before
//  it stores the squared 0x70003A24 and compares with the stored value (a
//  mov.s of it); mwcc 2.3.3 stores first and loads 0x70003A20 after, which
//  also swaps two FPRs. Assignment-in-condition, local, operand-order and 50
//  * 50 / x * x spellings were tried (best 99.41, same size).
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
#define F(p, o) (*(float *)((p) + (o)))
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern unsigned char D_70003B92[];
extern int D_70003B68[];
extern short D_70003B8A[];
extern unsigned char D_00810805;
extern unsigned char D_00810700;
extern float D_00810350[];
extern float D_overlay_AREA17_00828E10[];
extern float D_overlay_AREA17_00828E20[];
extern int func_001B0FD0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001C6380(unsigned char *self);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, void *v);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102918(void *dst, void *src, void *v);
extern void func_001F0460(int id, void *m);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_001B1D20(unsigned char *self);
extern int func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001EFD20(int id, void *pos);
extern void func_001FC580(unsigned char *self, int id);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern int func_0019AD00(unsigned char *self, void *pos, int flags);
extern int func_0019AB20(unsigned char *self, void *pos, void *dir, int flags);
extern int func_001B0D80(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00825520(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    unsigned char st5;
    short n;
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
                func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                func_00102918(D_700036A0, D_700036A0, self + 0xB0);
                D_700036A0[13] += 0.2f;
                func_001F0460(4, D_700036A0);
            }
        } else if (D_00810805 >= 3) {
            self[0] = 2;
            self[4] = 3;
        }
        (*(ActorFn *)(self + 0x4C))(self);
        func_001028D0(D_700038A0, D_00810350, self + 0xB0);
        *(int *)0x700038AC = 0;
        *(float *)0x70003A20 = func_00102738(D_700038A0, D_700038A0);
        {
            float k = 50.0f;
            *(float *)0x70003A24 = k;
            if (!(*(float *)0x70003A20 <= (*(float *)0x70003A24 = *(float *)0x70003A24 * k))) {
                func_001B17A0(self);
            } else {
                self[1] = 1;
                func_001B1D20(self);
            }
        }
        break;
    case 2:
        switch (st5 = self[5]) {
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
            F(self, 0x38) = D_overlay_AREA17_00828E10[(func_00122BB8() & 0x300) >> 8];
            F(blk, 0x78) = D_overlay_AREA17_00828E20[(func_00122BB8() & 0x300) >> 8];
            break;
        case 1:
            if (--S16(0x28) == 0) {
                F(self, 0xC4) = F(blk, 0x74);
                if (self[3] == 0x18 || self[3] == 0x2A) {
                    self[4] = 3;
                    self[5] = 0;
                } else {
                    self[5] = self[5] + 1;
                    S16(0x28) = 8;
                }
            }
            break;
        case 2:
            n = S16(0x28);
            if (n == 0) {
                self[5] = st5 + 1;
                self[0] = 1;
                S16(0x36) = 0;
                S16(0x34) = 1;
            } else {
                S16(0x28) = n - 1;
            }
        case 3:
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
            if (self[5] == 3 && S16(0x36) != 0) {
                if (!((short)S16(0x36) & 0x2000)) {
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
                    self[0] = 2;
                    break;
                }
                S16(0x36) = 0;
            }
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
                } else if ((((D_70003B68[0] + D_70003B8A[0]) & 0x3F) != 0 || func_001B0D80(self) == 0) &&
                           D_00810700 == 0x15 && F(self, 0xB4) < 5.0f) {
                    D_700038A0[0] = F(self, 0xB0);
                    D_700038A0[1] = 10.0f;
                    D_700038A0[2] = F(self, 0xB8);
                    D_700038A0[3] = 1.0f;
                    func_001EFD20(0x8000005F, D_700038A0);
                    func_001FBD50(self, 0xDB, 0, 800.0f);
                    S16(0x36) = 0;
                    self[4] = 3;
                    self[5] = 0;
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
