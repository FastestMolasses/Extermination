// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.50% NEARMISS).
// Fixes read from the original instructions:
// - func_00128250 is libgcc __fixunssfsi (float -> unsigned int): it takes only
//   the float in $f12. The old two-argument extern was wrong.
// - The lane-indexed int table D_00282178 was indexed as an int pointer plus
//   arg0 * 4 (a 16-byte stride); the original stores at arg0 * 4 bytes.
// - D_00275B2C is a small-data (gp-relative) global: the file builds at
//   -sdatathreshold 4, and the other globals are declared as unsized arrays so
//   they keep their absolute addressing.
// - The two early exits are one combined test (flag set OR clip zero), which
//   gives the original's branch-likely into the body and its two exit paths.
// - The active-flag byte is indexed directly at the test and at the final set
//   (no pointer local); that gives the original's saved-register order.
// - Inside the two arms the lane offset arg0 * 0x60 is written in place (no
//   shared local), and the start value is the base plus the clip's first word
//   with an explicit (int) on the clip word, which fixes the add operand order.
//
// Lane/clip activation routine. Indexed by arg0 (lane id) into the active-flag
// byte table D_00282154; returns immediately if the lane is already active or
// arg1 (clip id) is 0. Records arg1 into D_00282178[arg0]. For arg0!=0 it reads
// clip params from D_0025E170[arg1] and seeds the per-lane (0x60-stride) state
// arrays at D_00281Fxx/D_002820xx, computing a start value
// (unsigned)(60*(0.0746667*(field8>>11)) - 30); for arg0==0 it uses the
// D_0025DD30 table, records the active clip in D_00275B2C, and halves the angle
// term. Then fills the timing/flag fields (1,2,1,1 markers), copies fields, and
// computes a wrap value (>>11 of either half-of-field2 or field8, logical vs
// signed by a magnitude test), marks the lane active, and kicks func_001FABF0.

extern unsigned int func_00128250(float);
extern void func_001FABF0(int, int, int, int);
extern int D_0025DD30[][4];
extern int D_0025E170[][4];
extern int D_00275B2C;
extern char D_00281FD0[];
extern char D_00281FD1[];
extern char D_00281FD2[];
extern char D_00281FD3[];
extern char D_00281FE4[];
extern char D_00281FE8[];
extern char D_00281FF0[];
extern char D_00281FF4[];
extern char D_00281FF8[];
extern char D_00281FFC[];
extern char D_00282000[];
extern char D_00282004[];
extern char D_00282008[];
extern char D_0028201C[];
extern char D_00282020[];
extern char D_00282154[];
extern int D_00282178[];
extern int D_00282188[];
extern int D_0028218C[];
extern int D_00810E90[];

#define LANE_I(sym, b) (*(int *)(sym + (b)))
#define LANE_B(sym, b) (*(unsigned char *)(sym + (b)))

void func_001FA790(int arg0, int arg1) {
    int *src;
    int off;
    unsigned int half;
    unsigned int f8;

    if (D_00282154[arg0] != 0 || arg1 == 0) {
        return;
    }
    D_00282178[arg0] = arg1;
    if (arg0 != 0) {
        src = D_0025E170[arg1];
        LANE_I(D_00281FFC, arg0 * 0x60) = D_0028218C[0] + (int)src[0];
        LANE_I(D_00282020, arg0 * 0x60) = D_00810E90[0];
        LANE_I(D_0028201C, arg0 * 0x60) = func_00128250(60.0f * (0.074666664f * (float)(src[2] >> 11)) - 30.0f);
    } else {
        src = D_0025DD30[arg1];
        D_00275B2C = arg1;
        LANE_I(D_00281FFC, arg0 * 0x60) = D_00282188[0] + (int)src[0];
        LANE_I(D_00282020, arg0 * 0x60) = D_00810E90[0];
        LANE_I(D_0028201C, arg0 * 0x60) = func_00128250(60.0f * (0.074666664f * (float)(src[2] >> 11) / 2.0f) - 30.0f);
    }
    off = arg0 * 0x60;
    LANE_I(D_00281FF0, off) = src[3];
    LANE_I(D_00281FF4, off) = LANE_I(D_00281FFC, off);
    LANE_I(D_00281FF8, off) = ((src[2] + 0x7FF) >> 11) + LANE_I(D_00281FFC, off);
    LANE_B(D_00281FD0, off) = 1;
    LANE_B(D_00281FD2, off) = 1;
    LANE_B(D_00281FD1, off) = 2;
    LANE_B(D_00281FD3, off) = 1;
    LANE_I(D_00282008, off) = LANE_I(D_00281FE4, off);
    LANE_I(D_00282000, off) = LANE_I(D_00281FFC, off);
    half = (unsigned int)LANE_I(D_00281FE8, off) >> 1;
    f8 = (unsigned int)src[2];
    if (half < f8) {
        LANE_I(D_00282004, off) = (half + 0x7FF) >> 11;
    } else {
        LANE_I(D_00282004, off) = (int)(f8 + 0x7FF) >> 11;
    }
    D_00282154[arg0] = 1;
    func_001FABF0(arg0, arg1, 0, 0);
}
