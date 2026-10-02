// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00824490 (splat/link name 00824450; overlay code
//  is linked 0x40 below where it runs), 0x144 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Covers the splat pieces 00824450, 00824490 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x824210 (+5 1 of the script started at spawn entry 6): a
//  +0x28 frame timeline of func_001C67E0 motions: 20 -> (8, 0, 0); 179 ->
//  position (305.8422, 220, 600.1731), +0xC4 = -0.890118, (4, 0, 0); 2000 ->
//  (7, 20, 0); 2180 -> (0, 20, 0); 3100 -> (0xA, 20, 0); from 3100 on +0xB0
//  -= 0.07 and +0xB8 += 0.07 a frame.
#define S16(o) (*(short *)(self + (o)))
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);

void func_overlay_AREA08_00824450(unsigned char *self) {
    S16(0x28)++;
    if (S16(0x28) == 20) {
        func_001C67E0(self, 8, 0.0f, 0.0f);
    }
    if (S16(0x28) == 179) {
        *(float *)(self + 0xB0) = 305.8422f;
        *(float *)(self + 0xB4) = 220.0f;
        *(float *)(self + 0xB8) = 600.1731f;
        *(float *)(self + 0xC4) = -0.890118f;
        func_001C67E0(self, 4, 0.0f, 0.0f);
    }
    if (S16(0x28) == 2000) {
        func_001C67E0(self, 7, 20.0f, 0.0f);
    }
    if (S16(0x28) == 2180) {
        func_001C67E0(self, 0, 20.0f, 0.0f);
    }
    if (S16(0x28) >= 3100) {
        if (S16(0x28) == 3100) {
            int zi = 0;
            float z = (float)zi;
            func_001C67E0(self, 0xA, 20.0f, z);
        }
        *(float *)(self + 0xB0) -= 0.07f;
        *(float *)(self + 0xB8) += 0.07f;
    }
}
