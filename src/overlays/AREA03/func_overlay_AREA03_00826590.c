// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x008265D0 (splat/link name 00826590; overlay code
//  is linked 0x40 below where it runs), 0x290 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 0 placement [12]; 0x826340 with model 0x828FD0, the marker at
//  (535.5598, -10.0981, 423.631) and every D_00275CA0 choice swapped (+0 =
//  1 when set, script 0x828CD0 when set, marker (0, 0x80, 0, 0x80) when
//  set).
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern int D_00275CA0;
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA03_00828FD0[];
extern char D_overlay_AREA03_00828BD0[];
extern char D_overlay_AREA03_00828CD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *pos, float yaw);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern int func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *rgba);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_00826590(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA03_00828FD0;
        break;
    case 1:
        if (D_00275CA0 != 0) {
            self[0] = 1;
        } else {
            self[0] = 2;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (D_00275CA0 != 0) {
                    func_001BA1A0(talk, D_overlay_AREA03_00828CD0);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA03_00828BD0);
                }
                D_700038A0[0] = 0.0f;
                D_700038A0[1] = 0.0f;
                D_700038A0[2] = 5.6f;
                D_700038A0[3] = 1.0f;
                func_001B6F00(self, D_700038A0, -2.9670596f);
                S16(0x28) = 0;
                self[5] = 1;
            }
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
            break;
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        D_700038A0[0] = 535.5598f;
        D_700038A0[1] = -10.0981f;
        D_700038A0[2] = 423.631f;
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
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
