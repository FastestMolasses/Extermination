// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00825980 (splat/link name 00825940; overlay code
//  is linked 0x40 below where it runs), 0x470 bytes.
// Byte-identical at link (jump table pinned; tools/overlay/overlay_match.py
//  check AREA03 reports 99.99, rodata-needs-pin only; lane A03C).
// Role: sub 0 placement [9]. State 0: func_001C6380, state 1, +0 = 1, +8 =
//  1, +0x30 = 0x828BB0. +5 0: on +0xB bit 2 script 0x8282D0,
//  func_001B6F00(self, (0, 0, 6, 1), -2.9670596), +0x28 = 0, +5 = 1; then a
//  func_001F4BF0 marker at (550.1955, 94.9041, 413.1205), colour (0x80, 0,
//  0, 0x80) when D_70003B92 is set, else (0, 0x80, 0, 0x80). +5 1 (and above): +0x28 counts to
//  120 (func_001FBD50(self, 0x19A, 0, 300.0) at 120); +0xB and +5 are
//  cleared at the script end; the marker is (0, 0x80, 0, 0x80) with D_00275CA0
//  set, else (0x80, 0, 0, 0x80). Then the +0x4C method when func_001B17A0 is nonzero, and, when
//  D_008106A0 <= -2.4434612 or >= 0.69813174, func_001F4E20 at one of three
//  points chosen by D_00275CA4 (0 / 5, 1 / 4, other), colour (0x80, 0x40, 0,
//  0x80), 1.0. The switch on D_00275CA4 is a jump table at 0x829080
//  (tools/overlay/jt_pin.py). States 2, 3 and above: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern int D_00275CA0;
extern int D_00275CA4;
extern float D_008106A0[];
extern unsigned char D_70003B92[];
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA03_00828BB0[];
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

void func_overlay_AREA03_00825940(unsigned char *self) {
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
        *(char **)(self + 0x30) = D_overlay_AREA03_00828BB0;
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
                S16(0x28) = 0;
                self[5] = 1;
            }
            D_700038A0[0] = 550.1955f;
            D_700038A0[1] = 94.9041f;
            D_700038A0[2] = 413.1205f;
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
                self[0xB] = 0;
                self[5] = 0;
            }
            D_700038A0[0] = 550.1955f;
            D_700038A0[1] = 94.9041f;
            D_700038A0[2] = 413.1205f;
            D_700038A0[3] = 1.0f;
            if (D_00275CA0 != 0) {
                D_700038B0[0] = 0;
                D_700038B0[1] = 0x80;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
            } else {
                D_700038B0[0] = 0x80;
                D_700038B0[1] = 0;
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
        if (a <= -2.4434612f || !(a < 0.69813174f)) {
            switch (D_00275CA4) {
            case 0:
            case 5:
                D_700038A0[0] = 546.0836f;
                D_700038A0[1] = 103.1611f;
                D_700038A0[2] = 417.6306f;
                D_700038A0[3] = 1.0f;
                break;
            case 1:
            case 4:
                D_700038A0[0] = 545.6658f;
                D_700038A0[1] = 103.1611f;
                D_700038A0[2] = 418.0398f;
                D_700038A0[3] = 1.0f;
                break;
            case 2:
            case 3:
            default:
                D_700038A0[0] = 545.2155f;
                D_700038A0[1] = 103.1611f;
                D_700038A0[2] = 418.4036f;
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
