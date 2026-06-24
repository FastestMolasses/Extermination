// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Map the current "scene/level descriptor" (global record D_008102B0) to a small
// state code, only while the active-flag byte D_008102B4 == 1 (else 0). Dispatches
// on the descriptor's type byte at +5 (29/31, 30/32, 35, else), then refines on
// the sub-state bytes at +0x1F1 and +0x318:
//   type 29/31: if +0x1F1==1 -> {+0x318==1:2, ==3:0, else:1}; else +0x318==2?1:0
//   type 30/32: if +0x1F1==1 -> {+0x318==1:1, ==3:0, else:2}; else +0x318==2?2:0
//   type 35:    +0x1F1==1 ? 0x82 : 0
//   else:       (type==25) ? 3 : 0
//
// The trailing (v==25)?3:0 must be a ternary (not if/return) so mwcc fills the
// bne delay slot with the zero-store the way the target does. D_008102B0/B4 stay
// hi/lo (sized [16]). mwcc 2.3.3 (mwcps2-2.3.3-000906): pinned 991202 caps at
// 76.0% here. Verified objdiff 100% vs build/expected/func_0015D2F0.o.
extern unsigned char D_008102B0[16];
extern unsigned char D_008102B4[16];

int func_0015D2F0(void) {
    unsigned char *p = D_008102B0;
    unsigned char v;

    if (D_008102B4[0] == 1) {
        v = p[5];
        if (v == 29 || v == 31) {
            if (p[0x1F1] == 1) {
                if (p[0x318] == 1) return 2;
                if (p[0x318] == 3) return 0;
                return 1;
            }
            if (p[0x318] == 2) return 1;
            return 0;
        }
        if (v == 30 || v == 32) {
            if (p[0x1F1] == 1) {
                if (p[0x318] == 1) return 1;
                if (p[0x318] == 3) return 0;
                return 2;
            }
            if (p[0x318] == 2) return 2;
            return 0;
        }
        if (v == 35) {
            if (p[0x1F1] == 1) return 0x82;
            return 0;
        }
        return (v == 25) ? 3 : 0;
    }
    return 0;
}
