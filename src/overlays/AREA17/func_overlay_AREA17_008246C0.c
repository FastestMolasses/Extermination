// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00824700 (splat/link name 008246C0; overlay code
//  is linked 0x40 below where it runs), 0x5F0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [34]: the item-exchange NPC (counters 0x2E / 0x2F).
//  State 0: state 3 when func_001BA1C0(self, 0x2F); else the model setup
//  (func_001B10B0 0x56, func_001C63E0 9, func_001CA6F0 2, func_001BA8E0),
//  +0x58 = D_0028A5EC, +0x30 = 0x828390, state 1. State 1 (while D_00810785
//  is set): with D_00810786 == 1 script 0x8275E0, at its end +0x40 =
//  D_0028A5E8, func_001C67E0(self, 0xA, 0, 0), D_00810806 = 0xFF,
//  func_001FAE70(0), func_0019C6F0(0xF, 0). Else by D_00810807: 0: with the
//  player in area 0x8283A0 (kind 4) script 0x827A40, at its end
//  func_001AEE10(4, 0), func_001C47E0(0x28, 1) (takes item 0x28),
//  func_001C67E0(self, 4, 0, 0), D_00810807 = 1; 1: on +0xB bit 2 script
//  0x827E00, at its end +0xC4 = 1.466076 and func_001C67E0(self, 4, 0, 0); 2
//  / 3: D_00810787 = 1, script 0x828050, at its end func_001C47A0(0x2B, 1)
//  (gives item 0x2B), func_001C4760(0x19, 1), func_001B6660(group 0x826560),
//  D_00810807 = 0xFF, state 3; while it runs a func_001F9100 marker at the
//  +0x114 object's +0xC0 plus (2, -13.7, -3). Then func_001C68C0,
//  func_001B17A0, +1 = 1 and the +0x4C method. States 2 / 3 func_001BA540 and
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
#define SPF(a) (*(float *)(a))
#define SPI(a) (*(int *)(a))
typedef struct {
    float v[16];
} Area __attribute__((aligned(16)));
extern Area D_overlay_AREA17_008283A0;
extern char D_overlay_AREA17_00828390[];
extern char D_overlay_AREA17_008275E0[];
extern char D_overlay_AREA17_00827A40[];
extern char D_overlay_AREA17_00827E00[];
extern char D_overlay_AREA17_00828050[];
extern char D_overlay_AREA17_00826560[];
extern int D_0028A5E8;
extern int D_0028A5EC;
extern float D_00810350[];
extern unsigned char D_00810785;
extern unsigned char D_00810786;
extern unsigned char D_00810787;
extern unsigned char D_00810806;
extern unsigned char D_00810807;
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001BA580(unsigned char *self, int a1);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001FAE70(int a);
extern void func_0019C6F0(int id, int on);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001AEE10(int a, int b);
extern void func_001C47E0(int id, int n);
extern void func_001C47A0(int id, int n);
extern void func_001C4760(int id, int n);
extern void *func_001B6660(void *grp);
extern void func_00102948(void *dst, void *src);
extern void func_001F9100(void *pos, void *a, void *b, float f12);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_008246C0(unsigned char *self) {
    Area area = D_overlay_AREA17_008283A0;
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2F) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x56);
        func_001C63E0(self, 9);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        *(int *)(self + 0x58) = D_0028A5EC;
        *(char **)(self + 0x30) = D_overlay_AREA17_00828390;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (D_00810785 == 0) {
            break;
        }
        if (D_00810786 == 1) {
            switch (self[5]) {
            case 0:
                func_001BA1A0(talk, D_overlay_AREA17_008275E0);
                self[5] = 1;
                func_001BA580(self, self[0xD]);
                func_001C64F0(self, 1.0f);
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    int zi = 0;
                    float z = (float)zi;
                    self[5] = 0;
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    *(int *)(self + 0x40) = D_0028A5E8;
                    func_001C67E0(self, 0xA, 0.0f, z);
                    D_00810806 = 0xFF;
                    func_001FAE70(0);
                    func_0019C6F0(0xF, 0);
                }
                func_001BA580(self, self[0xD]);
                *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
                break;
            }
            func_001C68C0(self);
            func_001B17A0(self);
            self[1] = 1;
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        }
        switch (D_00810807) {
        case 0:
            switch (self[5]) {
            case 0:
                if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
                    func_001BA1A0(talk, D_overlay_AREA17_00827A40);
                    self[5] = 1;
                    S16(0x2E) = 0;
                }
                func_001BA580(self, self[0xD]);
                func_001C64F0(self, 1.0f);
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    func_001AEE10(4, 0);
                    func_001C47E0(0x28, 1);
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    self[5] = 0;
                    *(int *)(self + 0x40) = D_0028A5E8;
                    func_001C67E0(self, 4, 0.0f, 0.0f);
                    func_001FAE70(0);
                    D_00810807 = 1;
                }
                func_001BA580(self, self[0xD]);
                *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
                break;
            }
            break;
        case 1:
            switch (self[5]) {
            case 0:
                if (self[0xB] & 4) {
                    func_001BA1A0(talk, D_overlay_AREA17_00827E00);
                    self[5] = 1;
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    *(float *)(self + 0xC4) = 1.466076f;
                    func_001C67E0(self, 4, 0.0f, 0.0f);
                    self[0xB] = 0;
                    self[5] = 0;
                }
                break;
            }
            func_001BA580(self, self[0xD]);
            func_001C64F0(self, 1.0f);
            break;
        case 2:
        case 3:
            switch (self[5]) {
            case 0:
                D_00810787 = 1;
                func_001BA1A0(talk, D_overlay_AREA17_00828050);
                S16(0x2E) = 0;
                self[5] = 1;
                func_001BA580(self, self[0xD]);
                func_001C64F0(self, 1.0f);
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    func_001C47A0(0x2B, 1);
                    func_001C4760(0x19, 1);
                    func_001B6660(D_overlay_AREA17_00826560);
                    D_00810807 = 0xFF;
                    func_001FAE70(0);
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    self[4] = 3;
                }
                func_001BA580(self, self[0xD]);
                *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
                func_00102948(D_700038A0, *(unsigned char **)(self + 0x114) + 0xC0);
                D_700038A0[0] += 2.0f;
                D_700038A0[1] -= 13.7f;
                D_700038A0[2] -= 3.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 0.0f;
                D_700038B0[1] = 1.0f;
                D_700038B0[2] = 0.0f;
                D_700038B0[3] = 1.0f;
                func_001F9100(D_700038A0, D_700038A0, D_700038B0, 5.0f);
                break;
            }
            break;
        }
        func_001C68C0(self);
        func_001B17A0(self);
        self[1] = 1;
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
