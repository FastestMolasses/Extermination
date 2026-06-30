// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Display/render-mode reconfigure. Early-out if the global enable byte
// D_00821058 == 1. Otherwise notify func_001F9CF0(mode), and if the
// stored mode byte D_0028215B differs from the requested D_0081011C,
// commit it and reprogram two scanout/CRTC channels via func_0011A608
// (arg = 1 << dynamic-shift D_00281FD4 / D_002820F4; the two 0x3FFF/0
// vs 0x3000/0x3000 windows select progressive vs interlaced). Then
// copy a 0xC0-byte config block (D_00281C30 <- D_00281B70) and kick
// func_001FC6E0. Verified objdiff 100.0% vs build/expected/func_001FB100.o
// (mwcc 2.3.3 / mwcps2-2.3.3-000906); pinned 991202 walls at 95.45%.
extern void func_001F9CF0(unsigned char);
extern void func_00119870(int);
extern void func_0011A608(long long, int, int);
extern void block_copy(void *, void *, int);
extern void func_001FC6E0(void);
extern unsigned char D_00821058;
extern unsigned char D_0028215B;
extern unsigned char D_0081011C;
extern int D_00281FD4;
extern int D_002820F4;
extern char D_00281C30;
extern char D_00281B70;

void func_001FB100(void) {
    if (D_00821058 != 1) {
        func_001F9CF0(D_00821058);
        if (D_0028215B != D_0081011C) {
            D_0028215B = D_0081011C;
            if ((D_0081011C & 0xFF) == 0) {
                func_00119870(0);
                func_0011A608(1LL << D_00281FD4, 0x3FFF, 0);
                func_0011A608(1LL << D_002820F4, 0, 0x3FFF);
            } else {
                func_00119870(1);
                func_0011A608(1LL << D_00281FD4, 0x3000, 0x3000);
                func_0011A608(1LL << D_002820F4, 0x3000, 0x3000);
            }
        }
        block_copy(&D_00281C30, &D_00281B70, 0xC0);
        func_001FC6E0();
    }
}
