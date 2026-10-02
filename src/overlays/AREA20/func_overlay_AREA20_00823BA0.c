// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823BE0 (splat/link name 00823BA0; overlay code
//  is linked 0x40 below where it runs), 0x144 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Covers the splat pieces 00823BA0, 00823BE0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8239B0 once D_0081080C is set. +5 0: with the player in
//  the area 0x8273B0 (func_001B1EA0 kind 4, copied to the stack) D_0081080C =
//  2, script 0x8270B0, +5 1; func_001C64F0(self, 1.0). +5 1: func_001BA580;
//  at the script end func_001B0C60(0x15, 0, 3) (AREA21 entry 3), +0x2E =
//  0xFFFF, state 3; the +0x1F0 work's +0xE = func_001C64F0(self, 0.5). Then
//  func_001C68C0, func_001B17A0, +1 = 1 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
typedef struct {
    float v[16];
} Area __attribute__((aligned(16)));
extern Area D_overlay_AREA20_008273B0;
extern float D_00810350[];
extern unsigned char D_0081080C;
extern char D_overlay_AREA20_008270B0[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern void func_001BA580(unsigned char *self, int a1);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B0C60(int area, int sub, int entry);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);

void func_overlay_AREA20_00823BA0(unsigned char *self) {
    Area area = D_overlay_AREA20_008273B0;
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
            D_0081080C = 2;
            func_001BA1A0(talk, D_overlay_AREA20_008270B0);
            self[5] = 1;
        }
        func_001C64F0(self, 1.0f);
        break;
    case 1:
        func_001BA580(self, self[0xD]);
        if (func_001BA1F0(self) != 0) {
            func_001B0C60(0x15, 0, 3);
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            self[4] = 3;
        }
        *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
        break;
    }
    func_001C68C0(self);
    func_001B17A0(self);
    self[1] = 1;
    (*(ActorFn *)(self + 0x4C))(self);
}
