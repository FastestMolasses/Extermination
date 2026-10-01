// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00825DF0 (splat/link name 00825DB0; overlay code
//  is linked 0x40 below where it runs), 0x478 bytes.
// Byte-identical at link (jump table pinned; tools/overlay/overlay_match.py
//  check AREA03 reports 99.99, rodata-needs-pin only; lane A03C).
// Role: sub 0 placement [10]; 0x825980 with model 0x828B90, other points,
//  the angle bounds -2.268928 / 0.87266463, D_00275CA4 = 0 when its script
//  starts and when it ends, and the +5 1 marker (0x80, 0, 0, 0x80) with D_00275CA0 set. The
//  D_00275CA4 switch is a jump table at 0x8290A0 (jt_pin.py).
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern int D_00275CA0;
extern int D_00275CA4;
extern float D_008106A0[];
extern unsigned char D_70003B92[];
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA03_00828B90[];
extern char D_overlay_AREA03_008282D0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *pos, float yaw);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern int func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *rgba);
extern void func_001F4E20(void *pos, void *rgba, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_00825DB0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    float a;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA03_00828B90;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(talk, D_overlay_AREA03_008282D0);
                D_700038A0[0] = 0.0f;
                D_700038A0[1] = 0.0f;
                D_700038A0[2] = 6.0f;
                D_700038A0[3] = 1.0f;
                func_001B6F00(self, D_700038A0, -2.9670596f);
                D_00275CA4 = 0;
                S16(0x28) = 0;
                self[5] = 1;
            }
            D_700038A0[0] = 550.2136f;
            D_700038A0[1] = -10.0981f;
            D_700038A0[2] = 413.1457f;
            D_700038A0[3] = 1.0f;
            if (D_70003B92[0] != 0) {
                D_700038B0[0] = 0x80;
                D_700038B0[1] = 0;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
            } else {
                D_700038B0[0] = 0;
                D_700038B0[1] = 0x80;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
            }
            func_001F4BF0(D_700038A0, D_700038B0);
            break;
        case 1:
        default:
            if (S16(0x28) < 0x78) {
                S16(0x28)++;
                if (S16(0x28) == 0x78) {
                    func_001FBD50(self, 0x19A, 0, 300.0f);
                }
            }
            if (func_001BA1F0(self) != 0) {
                D_00275CA4 = 0;
                self[0xB] = 0;
                self[5] = 0;
            }
            D_700038A0[0] = 550.2136f;
            D_700038A0[1] = -10.0981f;
            D_700038A0[2] = 413.1457f;
            D_700038A0[3] = 1.0f;
            if (D_00275CA0 != 0) {
                D_700038B0[0] = 0x80;
                D_700038B0[1] = 0;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
            } else {
                D_700038B0[0] = 0;
                D_700038B0[1] = 0x80;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
            }
            func_001F4BF0(D_700038A0, D_700038B0);
            break;
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        a = D_008106A0[0];
        if (a <= -2.268928f || !(a < 0.87266463f)) {
            switch (D_00275CA4) {
            case 0:
            case 5:
                D_700038A0[0] = 542.1862f;
                D_700038A0[1] = -1.7694f;
                D_700038A0[2] = 421.4474f;
                D_700038A0[3] = 1.0f;
                break;
            case 1:
            case 4:
                D_700038A0[0] = 542.628f;
                D_700038A0[1] = -1.7694f;
                D_700038A0[2] = 421.0331f;
                D_700038A0[3] = 1.0f;
                break;
            case 2:
            case 3:
            default:
                D_700038A0[0] = 543.0248f;
                D_700038A0[1] = -1.7694f;
                D_700038A0[2] = 420.6201f;
                D_700038A0[3] = 1.0f;
                break;
            }
            D_700038B0[0] = 0x80;
            D_700038B0[1] = 0x40;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
            func_001F4E20(D_700038A0, D_700038B0, 1.0f);
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
