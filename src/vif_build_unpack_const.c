// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.42% NEARMISS).
// Fixes read from the original instructions:
// - Packet 1's quadword count n (8 matrix qwords) is an unsigned local set just
//   before the packet: the DMA tag's qwc is n + 1 (header included) and the
//   VIF UNPACK word is (n << 16) | 0x6C000000. The original keeps that sum and
//   that OR unfolded, which is why the NLOOP looked like "8 then +1"; an int n
//   or literal constants fold them.
// - Packet 2 addresses its header through s1 = p + 0x10 (zero the quadword at
//   s1, UNPACK word at s1 + 0xC, copy to s1 + 0x10), which keeps the copy
//   destination as two separate adds formed before the cursor store.
//
// Builds two GS/DMA packets in the per-slot display-list table. First initializes
// a global 0x80-byte block (D_00817240..D_008172BC): a 4x4 identity-ish transform
// (1.0 / -1.0 diagonal, 32.0 scale entries) plus three GS coordinate offsets
// (0x4B000040 / 0x4B000080). slot = D_00275670 (gp-rel base) indexed by arg0
// (word stride); the live packet pointer is slot[4] (offset 0x10). Packet 1 writes
// a tag header (byte +3 = 0x10, word +4 = 0, half +0 = qwc n + 1 = 9),
// advances slot[4] by 0xA0, zeroes the qword at +0x10, writes 0x11000000 at +0x18
// and the UNPACK word (num 8, addr 0) at +0x1C, then issues two matrix copies
// (copy_qw4) for the +0x20 and +0x60 sub-blocks (src = D_70003AC0 and the
// D_00817240 matrix). Packet 2 mirrors this with qwc 5, stride 0x60, UNPACK word
// 0x6C0403F5, one copy from the D_00817280 sub-block. copy_qw4 is a 2-arg qword
// matrix copy (a2/a3 at the call sites are register leftovers). The
// `volatile int [2]` declarations keep these globals out of .sdata (absolute
// %hi/%lo) and preserve the ascending-address store order.
typedef unsigned __int128 uint128;
extern int *D_00275670;

extern volatile int D_00817240[2];
extern volatile int D_00817244[2];
extern volatile int D_00817248[2];
extern volatile int D_0081724C[2];
extern volatile int D_00817250[2];
extern volatile int D_00817254[2];
extern volatile int D_00817258[2];
extern volatile int D_0081725C[2];
extern volatile int D_00817260[2];
extern volatile int D_00817264[2];
extern volatile int D_00817268[2];
extern volatile int D_0081726C[2];
extern volatile int D_00817270[2];
extern volatile int D_00817274[2];
extern volatile int D_00817278[2];
extern volatile int D_0081727C[2];
extern volatile int D_00817280[2];
extern volatile int D_00817284[2];
extern volatile int D_00817288[2];
extern volatile int D_0081728C[2];
extern volatile int D_00817290[2];
extern volatile int D_00817294[2];
extern volatile int D_00817298[2];
extern volatile int D_0081729C[2];
extern volatile int D_008172A0[2];
extern volatile int D_008172A4[2];
extern volatile int D_008172A8[2];
extern volatile int D_008172AC[2];
extern volatile int D_008172B0[2];
extern volatile int D_008172B4[2];
extern volatile int D_008172B8[2];
extern volatile int D_008172BC[2];

extern int D_70003AC0[2];
extern void copy_qw4(int dst, int src);
#define SQZERO(a) (*(uint128 *)(a) = 0)

void vif_build_unpack_const(int arg0) {
    int *slot;
    int p;
    int s1;
    unsigned int n;

    D_00817240[0] = 0x3F800000;
    D_00817244[0] = 0;
    D_00817248[0] = 0;
    D_0081724C[0] = 0;
    D_00817250[0] = 0;
    D_00817254[0] = 0xBF800000;
    D_00817258[0] = 0;
    D_0081725C[0] = 0;
    D_00817260[0] = 0;
    D_00817264[0] = 0x3F800000;
    D_00817268[0] = 0;
    D_0081726C[0] = 0;
    D_00817270[0] = 0;
    D_00817274[0] = 0;
    D_00817278[0] = 0;
    D_0081727C[0] = 0x3F800000;
    D_00817280[0] = 0x42000000;
    D_00817284[0] = 0x42000000;
    D_00817288[0] = 0x42000000;
    D_0081728C[0] = 0;
    D_00817290[0] = 0;
    D_00817294[0] = 0;
    D_00817298[0] = 0;
    D_0081729C[0] = 0;
    D_008172A0[0] = 0x42000000;
    D_008172A4[0] = 0x42000000;
    D_008172A8[0] = 0x42000000;
    D_008172AC[0] = 0;
    D_008172B0[0] = 0x4B000040;
    D_008172B4[0] = 0x4B000040;
    D_008172B8[0] = 0x4B000040;
    D_008172BC[0] = 0x4B000080;

    n = 8;
    slot = (int *)((char *)D_00275670 + arg0 * 4);
    *(unsigned char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = n + 1;
    p = slot[4];
    slot[4] = p + 0xA0;
    SQZERO(p + 0x10);
    *(int *)(p + 0x18) = 0x11000000;
    s1 = p + 0x10;
    *(int *)(p + 0x1C) = (n << 16) | 0x6C000000;  /* UNPACK, num = n, addr 0 */
    copy_qw4(s1 + 0x10, (int)D_70003AC0);

    copy_qw4(s1 + 0x50, (int)D_00817240);

    slot = (int *)((char *)D_00275670 + arg0 * 4);
    *(unsigned char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 5;
    p = slot[4];
    s1 = p + 0x10;
    slot[4] = p + 0x60;
    SQZERO(s1);
    *(int *)(s1 + 0xC) = 0x6C0403F5;
    copy_qw4(s1 + 0x10, (int)D_00817280);
}
