// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA07 overlay, runtime 0x00823990 (splat/link name 00823950; overlay code
//  is linked 0x40 below where it runs), 0x1A4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA07; lane OVLC).
// Role: sub 3 placement [7] (the counter 0x15 / flag 0x15 owner). State 0:
//  after func_001B0FD0, func_001C6380, +0x30 = 0x827170, state 1, +0 = 2 when
//  func_001BA1C0(self, 0x15) else 1. State 1, while D_0081076D != 0xFF: with
//  D_0081076D == 0 a func_001F4BF0 marker (0, 0x80, 0, 0x80) at +0xB0 plus
//  (0.4, 0, 0); then 0x823C20 when D_00810770 == 0xFF, else 0x823B40. Always
//  func_001C6380, func_001B17A0 and the +0x4C method. States 2 / 3
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern float D_700038A0[4];
extern int D_700038B0[4];
extern unsigned char D_0081076D;
extern unsigned char D_00810770;
extern char D_overlay_AREA07_00827170[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int flag);
extern void func_001F4BF0(void *pos, void *rgba);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA07_00823B40(unsigned char *self);
extern void func_overlay_AREA07_00823C20(unsigned char *self);

void func_overlay_AREA07_00823950(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        *(char **)(self + 0x30) = D_overlay_AREA07_00827170;
        self[4] = 1;
        if (func_001BA1C0(self, 0x15) != 0) {
            self[0] = 2;
        } else {
            self[0] = 1;
        }
        break;
    case 1:
        if (D_0081076D != 0xFF) {
            if (D_0081076D == 0) {
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
            if (D_00810770 == 0xFF) {
                func_overlay_AREA07_00823C20(self);
            } else {
                func_overlay_AREA07_00823B40(self);
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
