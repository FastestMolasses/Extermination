// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x008242F0 (splat/link name 008242B0; overlay
// code is linked 0x40 below where it runs), 0x510 bytes. Byte-identical
// once the link places the jump table (overlay_match.py: 99.99, the only
// difference is the unresolved table address; link_overlay.py AREA02 PASS).
// Covers the splat pieces 008242B0, 008242F0 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: returns 1 only when kind 7 finishes (+4 = 3), else 0. +0x28++.
//  Kinds 7/8: func_001C64F0(self, 1.0), func_001C68C0, the +0x4C callback;
//  kind 7 also func_001B1B70 and func_001A2370(self, bone +0x110[0] + 0x90).
//  Kind 7, +6 through a jump table (runtime 0x829200): 0 starts script
//  0x826980, +6 = 1, counter +0x2E0 = 0 and returns 0 at once (no +7
//  switch, none of the calls after it, no +0x2A increment); 1..5 turn +0xC4
//  with func_001B12B0 toward 0.314, 0.436, 0.349, 0.209 and -0.319 and advance at script end
//  (scripts 0x8269C0, 0x826A00, 0x826A40, 0x826A80); 6 sets D_008107E1 bit 7,
//  +6 = 7; 7 sets +4 = 3 and returns 1. Then +7: 0 once +0xB0 > -280 sets
//  D_008107E1 bit 2, +7 = 1 and plays func_001F02C0 0x8B3 and 0x8B4 (900);
//  1 once +0xB0 > 140 sets bit 3, +7 = 2, func_001F6B30, 0x8B5 (300) and
//  func_001EFD20(0, +0xB0); 2 counts +0x2E0. Then func_001AA700, 0x824CD0,
//  0x824910, 0x824AC0 and +0x2A++. Kind 8, +6: 0 script 0x826AC0 and counter
//  0; 1 counts, advances at script end and turns toward 0.534; 2 sets bit 6,
//  +6 = 3; 3 func_0019C6F0(1, 1), +4 = 3; then +0x2A++.
// Matching: the rate argument of the 4th turn and of the kind 8 turn, and the
//  two func_001F02C0 volumes 900 (second call) and 300, are staged through
//  block-local integers (idiom-31). The jump table links from C
//  (tools/overlay/jt_pin.py).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107E1;
extern char D_overlay_AREA02_00826980[];
extern char D_overlay_AREA02_008269C0[];
extern char D_overlay_AREA02_00826A00[];
extern char D_overlay_AREA02_00826A40[];
extern char D_overlay_AREA02_00826A80[];
extern char D_overlay_AREA02_00826AC0[];
extern int func_0019C6F0(int a0, int a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *a1);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001F02C0(float *, int, float);
extern void func_001F6B30(void);
extern char *func_001EFD20(int id, void *a);
extern void func_001AA700(unsigned char *self);
extern void func_overlay_AREA02_00824CD0(unsigned char *self);
extern void func_overlay_AREA02_00824910(unsigned char *self);
extern void func_overlay_AREA02_00824AC0(unsigned char *self);

int func_overlay_AREA02_008242B0(unsigned char *self) {
    int *cnt = (int *)(self + 0x2E0);
    unsigned char *talk = self + 0x1F0;
    float dt = 0.0f;
    (*(short *)(self + 0x28))++;
    if (self[0xD] == 7) {
        dt = 1.0f;
    }
    if (self[0xD] == 8) {
        dt = 1.0f;
    }
    if (self[0xD] == 7 || self[0xD] == 8) {
        func_001C64F0(self, dt);
        func_001C68C0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        if (self[0xD] == 7) {
            func_001B1B70(self);
            func_001A2370(self, *(char **)(self + 0x110) + 0x90);
        }
    }
    if (self[0xD] == 7) {
        switch (self[6]) {
        case 0:
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826980);
            self[6] = 1;
            *cnt = 0;
            return 0;
        case 1:
            if (func_001BA1F0(self)) {
                self[6] = 2;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_008269C0);
            }
            *(float *)(self + 0xC4) = func_001B12B0(0.31415927f, *(float *)(self + 0xC4), 0.0029670598f);
            break;
        case 2:
            *(float *)(self + 0xC4) = func_001B12B0(0.43633232f, *(float *)(self + 0xC4), 0.0029670598f);
            if (func_001BA1F0(self)) {
                self[6] = 3;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826A00);
            }
            break;
        case 3:
            *(float *)(self + 0xC4) = func_001B12B0(0.349065870f, *(float *)(self + 0xC4), 0.0029670598f);
            if (func_001BA1F0(self)) {
                self[6] = 4;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826A40);
            }
            break;
        case 4:
            {
                int one = 1;
                float rate = (float)one * 0.0059341195f;
                *(float *)(self + 0xC4) = func_001B12B0(0.20943952f, *(float *)(self + 0xC4), rate);
            }
            if (func_001BA1F0(self)) {
                self[6] = 5;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826A80);
            }
            break;
        case 5:
            *(float *)(self + 0xC4) = func_001B12B0(-0.319395244f, *(float *)(self + 0xC4), 0.00890117977f);
            if (func_001BA1F0(self)) {
                self[6] = 6;
            }
            break;
        case 6:
            D_008107E1 |= 0x80;
            self[6] = 7;
            break;
        case 7:
            self[4] = 3;
            return 1;
        }
        switch (self[7]) {
        case 0:
            if (*(float *)(self + 0xB0) > -280.0f) {
                D_008107E1 |= 4;
                self[7] = 1;
                func_001F02C0((float *)(self + 0xB0), 0x8B3, 900.0f);
                {
                    int n = 900;
                    float v = (float)n;
                func_001F02C0((float *)(self + 0xB0), 0x8B4, v);
                }
            }
            break;
        case 1:
            if (*(float *)(self + 0xB0) > 140.0f) {
                D_008107E1 |= 8;
                self[7] = 2;
                func_001F6B30();
                {
                    int n = 300;
                    float v = (float)n;
                func_001F02C0((float *)(self + 0xB0), 0x8B5, v);
                }
                func_001EFD20(0, self + 0xB0);
            }
            break;
        case 2:
            (*cnt)++;
            break;
        }
        func_001AA700(self);
        func_overlay_AREA02_00824CD0(self);
        func_overlay_AREA02_00824910(self);
        func_overlay_AREA02_00824AC0(self);
        (*(short *)(self + 0x2A))++;
        return 0;
    } else if (self[0xD] == 8) {
        switch (self[6]) {
        case 0:
            func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00826AC0);
            self[6] = 1;
            *cnt = 0;
            break;
        case 1:
            (*cnt)++;
            if (func_001BA1F0(self)) {
                self[6] = 2;
            }
            {
                int one = 1;
                float rate = (float)one * 0.0029670598f;
                *(float *)(self + 0xC4) = func_001B12B0(0.53407073f, *(float *)(self + 0xC4), rate);
            }
            break;
        case 2:
            D_008107E1 |= 0x40;
            self[6] = 3;
            break;
        case 3:
            func_0019C6F0(1, 1);
            self[4] = 3;
            break;
        }
        (*(short *)(self + 0x2A))++;
        return 0;
    }
    return 0;
}
