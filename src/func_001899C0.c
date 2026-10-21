// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_00189630
//
// Byte-matched (objdiff 100%), decomp lane round 7, with func_00189630
// compiled as a static function of this file. The original compiler saw that
// leaf earlier in the same translation unit and knew it never writes a0, so it
// sets no a0 for the func_00189730 call: self is still there (the former
// NEARMISS residual, 78.26%: mwcc reloaded a0 from the saved register).
// tools/decomp/tu_static.py removes the static copy after checking it equals
// the original func_00189630; the call links to that function's own unit.

//
// Runs func_00189630(self, D_008102B0) and func_00189730(self, D_008102B0),
// copies the matrix at D_008103D0[0] + 0x90 to self + 0xD0 and passes it to
// func_001C9610(D_00275B40, self[0xC], self + 0xD0).

extern unsigned char D_008102B0[8];
extern int D_008103D0[2];
extern int *D_00275B40;
static void func_00189630(unsigned char *a0, unsigned char *a1);  /* same-TU callee, see TU_STATIC */
#include "src/func_00189630.c"
extern void func_00189730(unsigned char *p, unsigned char *e);
extern void copy_qw4(void *dst, void *src);
extern void func_001C9610(int *a0, int a1, void *a2);

// The original sets no $a0 before the func_00189730 call: func_00189630 is a
// leaf that never writes $a0, so self is still there. The call therefore
// receives (self, D_008102B0). The TU_STATIC callee reproduces that.
void func_001899C0(unsigned char *self) {
    func_00189630(self, D_008102B0);
    func_00189730(self, D_008102B0);
    copy_qw4(self + 0xD0, (void *)(D_008103D0[0] + 0x90));
    func_001C9610(D_00275B40, self[0xC], self + 0xD0);
}
