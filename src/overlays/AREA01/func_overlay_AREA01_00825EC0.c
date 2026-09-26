// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825F00 (splat/link name 00825EC0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825950; runs script 0x82AD90, then spawns groups
//  0x829220/0x8291C0.
// Covers the splat pieces 00825EC0, 00825F00 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
extern char D_overlay_AREA01_0082AD90[];
extern char D_overlay_AREA01_00829220[];
extern char D_overlay_AREA01_008291C0[];
extern unsigned char D_008107DF;
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern int func_001C47A0(int a0, int a1);
extern int func_001C4760(int a0, int a1);
extern unsigned char *func_001B6660(void *p);
extern void func_001FAE70(int a0);

void func_overlay_AREA01_00825EC0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001BA1A0(talk, D_overlay_AREA01_0082AD90);
        self[5] = 1;
        *(short *)(self + 0x28) = 0;
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            D_008107DF = 0xFF;
            func_001C47A0(0x20, 1);
            func_001C4760(5, 1);
            func_001B6660(D_overlay_AREA01_00829220);
            func_001B6660(D_overlay_AREA01_008291C0);
            func_001FAE70(0);
            self[4] = 3;
        }
        break;
    }
}
