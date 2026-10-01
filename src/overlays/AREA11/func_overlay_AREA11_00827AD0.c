// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00827B10 (splat/link name 00827AD0; overlay code is
// linked 0x40 below where it runs), 0x534 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: switch with a light child. The flag is bit (+0x2E) of
// D_00810841[D_00810700]. State 0 sets +0xB4 and the floats 0x82A7C4 /
// 0x82AB14 / 0x82A844 / 0x82A944 from D_0081083A (190/190/205/245 when set,
// else 230/230/245/205), then (after func_001B0FD0) +0 = +8 = 1, +0x30 =
// 0x82AB10, func_001C6380, state 1, func_001A2370, and spawns a class 0xC
// child (+0xD 0x10, +0xA0..+0xAC = (1, 0, 0, 0.25), behaviour func_001C5760)
// kept at +0x2E4. State 1: +5 0 waits for bit 2 of +0xB and starts script
// 0x82A750 (flag set, +0x2A = 0) or 0x82A990 (+0x2A = 300); +5 1 counts +0x2A
// up to 120 (sound 0x19A at 120), and at the script end clears +5 and +0xB
// and, with the flag set, toggles D_0081083A, re-applies the heights,
// func_001C6380 and func_001A2370; it then copies (*(+0x110)) +0x90 into the
// child's (*(+0x110)) +0x90. Then func_001B17A0 and the +0x4C method, and
// +0x28 moves by 8 per frame toward 128 (flag set) or 0; the child's
// +0xA0..+0xAC become (0, +0x28 / 128, 0, 0.25) while +0x28 is nonzero, else
// (1, 0, 0, 0.25). State 3/other: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081083A;
extern unsigned char D_00810700;
extern unsigned char D_00810841[];
extern float D_overlay_AREA11_0082A7C4;
extern float D_overlay_AREA11_0082A844;
extern float D_overlay_AREA11_0082A944;
extern float D_overlay_AREA11_0082AB14;
extern char D_overlay_AREA11_0082AB10[];
extern char D_overlay_AREA11_0082A750[];
extern char D_overlay_AREA11_0082A990[];
extern char D_001C5760[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *mtx);
extern unsigned char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

#define F(p, o) (*(float *)((p) + (o)))
#define S16(o) (*(short *)(self + (o)))
#define U16(o) (*(unsigned short *)(self + (o)))
#define LIT(o) (D_00810841[D_00810700] & (1U << U16(o)))
#define LAMP (*(unsigned char **)(self + 0x2E4))

void func_overlay_AREA11_00827AD0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *o;
    unsigned char **slot;
    switch (self[4]) {
    case 0:
        if (D_0081083A != 0) {
            F(self, 0xB4) = 190.0f;
            D_overlay_AREA11_0082A7C4 = 190.0f;
            D_overlay_AREA11_0082AB14 = 190.0f;
            D_overlay_AREA11_0082A844 = 205.0f;
            D_overlay_AREA11_0082A944 = 245.0f;
        } else {
            F(self, 0xB4) = 230.0f;
            D_overlay_AREA11_0082A7C4 = 230.0f;
            D_overlay_AREA11_0082AB14 = 230.0f;
            D_overlay_AREA11_0082A844 = 245.0f;
            D_overlay_AREA11_0082A944 = 205.0f;
        }
        if (func_001B0FD0(self) != 0) {
            break;
        }
        self[0] = 1;
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA11_0082AB10;
        func_001C6380(self);
        self[4] = 1;
        func_001A2370(self, self + 0xD0);
        slot = (unsigned char **)((int *)(self + 0x1F0) + 0x3D);
        LAMP = 0;
        o = func_001AFA90(0xC);
        if (o != 0) {
            o[0x9A] = 0;
            o[3] = 0;
            *(short *)(o + 0x2E) = 0;
            o[0xD] = 0x10;
            *(unsigned short *)(o + 0xE) = 0xFFFF;
            *(short *)(o + 0x54) = 0;
            *(short *)(o + 0x56) = 0;
            F(o, 0xA0) = 1.0f;
            F(o, 0xA4) = 0.0f;
            F(o, 0xA8) = 0.0f;
            F(o, 0xAC) = 0.25f;
            func_00102948(o + 0xB0, self + 0xB0);
            func_00102948(o + 0xC0, self + 0xC0);
            *(char **)(o + 0x10) = D_001C5760;
            *slot = o;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (LIT(0x2E)) {
                    func_001BA1A0(talk, D_overlay_AREA11_0082A750);
                    self[5]++;
                    S16(0x2A) = 0;
                } else {
                    func_001BA1A0(talk, D_overlay_AREA11_0082A990);
                    self[5]++;
                    S16(0x2A) = 300;
                }
            }
            break;
        case 1:
            if (S16(0x2A) < 120) {
                S16(0x2A)++;
                if (S16(0x2A) == 120) {
                    func_001FBD50(self, 0x19A, 0, 300.0f);
                }
            }
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
                if (LIT(0x2E)) {
                    D_0081083A = !D_0081083A;
                    if (D_0081083A != 0) {
                        F(self, 0xB4) = 190.0f;
                        D_overlay_AREA11_0082A7C4 = 190.0f;
                        D_overlay_AREA11_0082AB14 = 190.0f;
                        D_overlay_AREA11_0082A844 = 205.0f;
                        D_overlay_AREA11_0082A944 = 245.0f;
                    } else {
                        F(self, 0xB4) = 230.0f;
                        D_overlay_AREA11_0082A7C4 = 230.0f;
                        D_overlay_AREA11_0082AB14 = 230.0f;
                        D_overlay_AREA11_0082A844 = 245.0f;
                        D_overlay_AREA11_0082A944 = 205.0f;
                    }
                    func_001C6380(self);
                    func_001A2370(self, self + 0xD0);
                }
            }
            func_00102958(*(unsigned char **)(LAMP + 0x110) + 0x90,
                          *(unsigned char **)(self + 0x110) + 0x90);
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        if (LIT(0x2E)) {
            if (S16(0x28) < 128) {
                S16(0x28) += 8;
                if (S16(0x28) > 128) {
                    S16(0x28) = 128;
                }
            }
            if (S16(0x28) != 0) {
                *(float *)0x70003A20 = (float)S16(0x28) / 128.0f;
                *(int *)(LAMP + 0xA0) = 0;
                F(LAMP, 0xA4) = *(float *)0x70003A20;
                F(LAMP, 0xA8) = 0.0f;
                F(LAMP, 0xAC) = 0.25f;
            } else {
                F(LAMP, 0xA0) = 1.0f;
                F(LAMP, 0xA4) = 0.0f;
                F(LAMP, 0xA8) = 0.0f;
                F(LAMP, 0xAC) = 0.25f;
            }
        } else {
            if (S16(0x28) > 0) {
                S16(0x28) -= 8;
                if (S16(0x28) < 0) {
                    S16(0x28) = 0;
                }
            }
            if (S16(0x28) != 0) {
                *(float *)0x70003A20 = (float)S16(0x28) / 128.0f;
                *(int *)(LAMP + 0xA0) = 0;
                F(LAMP, 0xA4) = *(float *)0x70003A20;
                F(LAMP, 0xA8) = 0.0f;
                F(LAMP, 0xAC) = 0.25f;
            } else {
                F(LAMP, 0xA0) = 1.0f;
                F(LAMP, 0xA4) = 0.0f;
                F(LAMP, 0xA8) = 0.0f;
                F(LAMP, 0xAC) = 0.25f;
            }
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
