// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823B70 (splat/link name 00823B30; overlay code is
// linked 0x40 below where it runs), 0xD0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 00823B30, 00823B70 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-behaviour at runtime 0x823B70 (D_008107D8 nonzero, bit 7
// clear). +5 0: when bit 2 of +0xB is set, starts script 0x828810 and +5 =
// 1; +5 1: at the script end clip 8 at 20.0, +0xB = 0, +5 = 0. Then
// func_001BA580, func_001C64F0(1.0), func_001B17A0, func_001C68C0 and the
// +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern char D_overlay_AREA11_00828810[];
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);
extern void func_001BA580(unsigned char *self, unsigned char a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);

void func_overlay_AREA11_00823B30(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            func_001BA1A0(talk, D_overlay_AREA11_00828810);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            int zi = 0;
            float z = (float)zi;
            func_001C67E0(self, 8, 20.0f, z);
            self[0xB] = 0;
            self[5] = 0;
        }
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
    func_001B17A0(self);
    func_001C68C0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
