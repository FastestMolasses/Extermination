// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823B00 (splat/link name 00823AC0; overlay code
//  is linked 0x40 below where it runs), 0xD4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Covers the splat pieces 00823AC0, 00823B00 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8239B0 while D_0081080C is 0. +5 0: func_001FABB0() and
//  script 0x826830 (+5 1); +5 1: func_001BA580, at the script end
//  func_001FB0B0(0), D_0081080C = 1, +5 0. Then func_001C64F0(self, 1.0),
//  func_001C68C0, func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081080C;
extern char D_overlay_AREA20_00826830[];
extern void func_001FABB0(void);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern void func_001BA580(unsigned char *self, int a1);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FB0B0(int a);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);

void func_overlay_AREA20_00823AC0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001FABB0();
        func_001BA1A0(talk, D_overlay_AREA20_00826830);
        self[5] = 1;
        break;
    case 1:
        func_001BA580(self, self[0xD]);
        if (func_001BA1F0(self) != 0) {
            func_001FB0B0(0);
            D_0081080C = 1;
            self[5] = 0;
        }
        break;
    case 2:
        break;
    }
    func_001C64F0(self, 1.0f);
    func_001C68C0(self);
    func_001B17A0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
