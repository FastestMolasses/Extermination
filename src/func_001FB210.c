// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Subsystem init/reset. Calls two no-arg init helpers (func_00119240,
// func_00119318), registers two slots via func_00119978(0/1, 0, 0), then
// runs a sequence of config calls: func_00119880(0x3C), func_00119390(0x880),
// and two (func_001197F8 + func_00119810 + func_00119828) triplets configuring
// channels 0 and 1 (masks 0x1FFFFF / 0x19FFFF, mode 4). Zeroes six ints
// (D_00281D30..D_00281D44), sets D_00282160 = -1, memset-fills two 0xC0-byte
// config blocks (D_00281C30, D_00281B70) to 0xFF via func_00121A28, mirrors the
// D_0081011C flag into D_0028215B and calls func_00119870(0/1) on it, then
// clears four byte flags (D_00282150..D_00282153).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 90.7% -- it fills the bnez delay slot of the D_0081011C test, while the
// target (and 2.3.3) leave it as the clean store/nop. objdiff 100.0 vs expected.
extern void func_00119240(void);
extern void func_00119318(void);
extern void func_00119390(int);
extern void func_001197F8(int, int);
extern void func_00119810(int, int);
extern void func_00119828(int, int, int);
extern void func_00119870(int);
extern void func_00119880(int);
extern void func_00119978(int, int, int);
extern void func_00121A28(void *, int, int);
extern char D_00281B70;
extern char D_00281C30;
extern int D_00281D30;
extern int D_00281D34;
extern int D_00281D38;
extern int D_00281D3C;
extern int D_00281D40;
extern int D_00281D44;
extern char D_00282150;
extern char D_00282151;
extern char D_00282152;
extern char D_00282153;
extern unsigned char D_0028215B;
extern int D_00282160;
extern unsigned char D_0081011C;

void func_001FB210(void) {
    func_00119240();
    func_00119318();
    func_00119978(0, 0, 0);
    func_00119978(1, 0, 0);
    func_00119880(0x3C);
    func_00119390(0x880);
    func_001197F8(0, 0x1FFFFF);
    func_00119810(0, 4);
    func_00119828(0, 0, 0);
    func_001197F8(1, 0x19FFFF);
    func_00119810(1, 4);
    func_00119828(1, 0, 0);
    D_00281D44 = 0;
    D_00281D40 = 0;
    D_00281D3C = 0;
    D_00281D38 = 0;
    D_00281D34 = 0;
    D_00281D30 = 0;
    D_00282160 = -1;
    func_00121A28(&D_00281C30, -1, 0xC0);
    func_00121A28(&D_00281B70, -1, 0xC0);
    D_0028215B = D_0081011C;
    if (D_0081011C == 0) {
        func_00119870(0);
    } else {
        func_00119870(1);
    }
    D_00282153 = 0;
    D_00282152 = 0;
    D_00282151 = 0;
    D_00282150 = 0;
}
