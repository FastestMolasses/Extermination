// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00825920 (splat/link name 008258E0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 waits on func_001B0F60(self, 6 or 7) depending on
//  D_0081075D and +2 bit 7 (bit 7 starts script 0x82A540); may call
//  func_001EFD20(0, (-7.55, -5.69, -1391.65)). State 1: +5 0 sets +0 from
//  D_008104E6 and waits on +0xB bit 2; +5 1 at script end calls
//  func_00182F90(D_008102B0, (x, D_00810354, z - 5)) and the same
//  func_001EFD20 call. While D_00810702 is 5 or 6 it animates and, for +2 bit
//  7, calls func_001F4E20 at 0x82A710 unless D_0081075D is 0xFF.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075D;
extern unsigned char D_0081075E;
extern unsigned char D_008104E6;
extern unsigned char D_00810702;
extern float D_00810354;
extern char D_008102B0[];
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA00_0082A540[];
extern char D_overlay_AREA00_0082A700[];
extern char D_overlay_AREA00_0082A710[];
extern int func_001B0F60(void *p, int a);
extern void func_001EFD20(int a, void *b);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_00182F90(void *a, void *b);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001F4E20(void *a0, void *a1, float f12);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_008258E0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (D_0081075D == 0xFF) {
            if (func_001B0F60(self, 6) == 0) {
                self[5] = 2;
                if (self[2] & 0x80) {
                    if (D_0081075E != 0xFF) {
                        D_700038A0[0] = -7.55f;
                        D_700038A0[1] = -5.69f;
                        D_700038A0[2] = -1391.65f;
                        D_700038A0[3] = 1.0f;
                        func_001EFD20(0, D_700038A0);
                    }
                }
            }
        } else if (self[2] & 0x80) {
            if (func_001B0F60(self, 7) == 0) {
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082A540);
                *(char **)(self + 0x30) = D_overlay_AREA00_0082A700;
                self[0] = 1;
            }
        } else {
            if (func_001B0F60(self, 6) == 0) {
                self[5] = 2;
            }
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_008104E6 != 0) {
                self[0] = 2;
            } else {
                self[0] = 1;
            }
            if (self[0xB] & 4) {
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001C64F0(self, 1.0f);
                self[5] = 2;
                D_700038A0[0] = *(float *)(self + 0xB0);
                D_700038A0[1] = D_00810354;
                D_700038A0[2] = *(float *)(self + 0xB8) - 5.0f;
                D_700038A0[3] = 1.0f;
                func_00182F90(D_008102B0, D_700038A0);
                D_700038A0[0] = -7.55f;
                D_700038A0[1] = -5.69f;
                D_700038A0[2] = -1391.65f;
                D_700038A0[3] = 1.0f;
                func_001EFD20(0, D_700038A0);
            }
            break;
        case 2:
            break;
        }
        if (D_00810702 == 5 || D_00810702 == 6) {
            func_001C68C0(self);
            func_001B17A0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            if (self[2] & 0x80) {
                func_001026A0(D_700038A0, self + 0xD0, D_overlay_AREA00_0082A710);
                if (D_0081075D != 0xFF) {
                    D_700038B0[0] = 0;
                    D_700038B0[1] = 0x80;
                    D_700038B0[2] = 0;
                    D_700038B0[3] = 0x80;
                    func_001F4E20(D_700038A0, D_700038B0, 2.0f);
                }
            }
        }
        break;
    case 2:
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
