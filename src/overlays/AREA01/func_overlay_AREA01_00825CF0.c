// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825D30 (splat/link name 00825CF0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x825950; runs scripts 0x82AC10/0x82AD10 and a
//  0x26C-frame timer.
// Covers the splat pieces 00825CF0, 00825D30 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef void (*ActorFn)(unsigned char *);
extern char D_overlay_AREA01_0082AC10[];
extern char D_overlay_AREA01_0082AD10[];
extern char D_008105E0[];
extern unsigned char D_008107DF;
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_overlay_AREA01_00826010(unsigned char *self);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern void func_001CA6F0(void *a0, unsigned char a1);

void func_overlay_AREA01_00825CF0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_00102948(D_008105E0, *(char **)(self + 0x18) + 0xB0);
        func_overlay_AREA01_00826010(self);
        func_001C64F0(self, 1.0f);
        if (D_008107DF == 0x10) {
            func_001BA1A0(talk, D_overlay_AREA01_0082AC10);
            func_001CA6F0(self, 2);
            self[5]++;
            *(float *)(self + 0xB0) = 0.0f;
            *(float *)(self + 0xB4) = 0.0f;
            *(float *)(self + 0xB8) = 0.0f;
            *(float *)(self + 0xBC) = 0.0f;
            *(float *)(self + 0xC0) = 0.0f;
            *(float *)(self + 0xC4) = 0.0f;
            *(float *)(self + 0xC8) = 0.0f;
            *(float *)(self + 0xCC) = 0.0f;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[5] = 0;
            D_008107DF = 0x40;
            (*(unsigned char **)(self + 0x1C))[4] = 3;
            func_001BA1A0(talk, D_overlay_AREA01_0082AD10, 3);
        }
        *(short *)(talk + 0xE) = func_001C64F0(self, 1.0f);
        *(short *)(self + 0x28) += 1;
        if (*(short *)(self + 0x28) >= 0x26C) {
            *(short *)(talk + 0xE) = 0x1000;
        }
        break;
    }
    func_001C68C0(self);
    self[1] = 1;
    (*(ActorFn *)(self + 0x4C))(self);
}
