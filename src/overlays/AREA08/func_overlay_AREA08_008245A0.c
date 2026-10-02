// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x008245E0 (splat/link name 008245A0; overlay code
//  is linked 0x40 below where it runs), 0x1A4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: sub 3 placement [2]; the AREA07 0x823990 C with flag 0x18, the roles
//  of D_0081076D / D_00810770 swapped and 0x824870 / 0x824790 as the two
//  halves.
typedef void (*ActorFn)(unsigned char *);
extern float D_700038A0[4];
extern int D_700038B0[4];
extern unsigned char D_00810770;
extern unsigned char D_0081076D;
extern char D_overlay_AREA08_00827160[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int flag);
extern void func_001F4BF0(void *pos, void *rgba);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA08_00824790(unsigned char *self);
extern void func_overlay_AREA08_00824870(unsigned char *self);

void func_overlay_AREA08_008245A0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        *(char **)(self + 0x30) = D_overlay_AREA08_00827160;
        self[4] = 1;
        if (func_001BA1C0(self, 0x18) != 0) {
            self[0] = 2;
        } else {
            self[0] = 1;
        }
        break;
    case 1:
        if (D_00810770 != 0xFF) {
            if (D_00810770 == 0) {
                D_700038A0[0] = *(float *)(self + 0xB0) + 0.4f;
                D_700038A0[1] = *(float *)(self + 0xB4);
                D_700038A0[2] = *(float *)(self + 0xB8);
                D_700038A0[3] = 1.0f;
                D_700038B0[0] = 0;
                D_700038B0[1] = 0x80;
                D_700038B0[2] = 0;
                D_700038B0[3] = 0x80;
                func_001F4BF0(D_700038A0, D_700038B0);
            }
            if (D_0081076D == 0xFF) {
                func_overlay_AREA08_00824870(self);
            } else {
                func_overlay_AREA08_00824790(self);
            }
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
