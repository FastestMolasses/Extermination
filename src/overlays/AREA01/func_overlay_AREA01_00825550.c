// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825590 (splat/link name 00825550; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825350; talk state machine that starts script 0x829FA0
//  and sets D_008107D9 = 0x81 when it ends.
// Covers the splat pieces 00825550, 00825590 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern int func_001C4760(int a0, int a1);
extern unsigned char D_00810759;
extern unsigned char D_008107D9;
extern char D_overlay_AREA01_00829FA0[];

void func_overlay_AREA01_00825550(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        *(float *)(talk + 0x44) = *(float *)(self + 0xC4);
        func_001C67E0(self, 1, 40.0f, 0.0f);
        self[5] = 1;
        break;
    case 1:
        if (self[0xB] & 4) {
            self[5] = 2;
            func_001BA1A0(talk, D_overlay_AREA01_00829FA0, 2);
        }
        break;
    case 2:
        if (func_001BA1F0(self) != 0) {
            D_00810759 = 0xFF;
            func_001C4760(2, 1);
            D_008107D9 = 0x81;
            self[5] = 0;
            self[6] = 0;
            self[0xB] = 0;
        }
        break;
    }
}
