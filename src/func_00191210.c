// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Conditional float clamp. When the global state byte D_00810700 == 0x10 and
// D_00810702 == 0 and D_0081078B != 0xFF, clamps two float globals up to a
// floor of 507.0f (0x43FD8000): if D_008101F8 < 507 set it to 507, likewise
// for D_008105D8. The constant is materialized as an integer (lui/ori) and the
// store reuses that integer register (mwcc's float-constant-store idiom).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 build reaches
// 90.0% (it fills the clean-store beqz/bc1f delay slot with the safe constant
// reload that 2.3.3 and the target leave as the natural fall-through).
extern float D_008101F8;
extern float D_008105D8;
extern unsigned char D_00810700;
extern unsigned char D_00810702;
extern unsigned char D_0081078B;

void func_00191210(void) {
    if (D_00810700 == 0x10 && D_00810702 == 0 && D_0081078B != 0xFF) {
        if (D_008101F8 < 507.0f) {
            D_008101F8 = 507.0f;
        }
        if (D_008105D8 < 507.0f) {
            D_008105D8 = 507.0f;
        }
    }
}
