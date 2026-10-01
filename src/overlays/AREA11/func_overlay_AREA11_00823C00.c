// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823C40 (splat/link name 00823C00; overlay code is
// linked 0x40 below where it runs), 0xA0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 00823C00, 00823C40 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-behaviour at runtime 0x823C40 (bit 7 of D_008107D8 set).
// +5 0 starts script 0x828A10 and +5 = 1; +5 1 at the script end calls
// func_001B0C60(1, 0, 4) and sets state 3. Then func_001B17A0,
// func_001C68C0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern char D_overlay_AREA11_00828A10[];
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001B0C60(int a0, int a1, int a2);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);

void func_overlay_AREA11_00823C00(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001BA1A0(talk, D_overlay_AREA11_00828A10);
        self[5] = 1;
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            func_001B0C60(1, 0, 4);
            self[4] = 3;
        }
        break;
    }
    func_001B17A0(self);
    func_001C68C0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}
