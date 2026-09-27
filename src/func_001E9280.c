// NEARMISS func_001E9280  (vram 0x001E9280, 0x2F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The body follows the original instructions;
// the residual diff is code generation only:
// Body corrected 2026-09-26 against the original instructions, as its twin func_001E9E60.c was (was 84.76% with the wrong body): segments s+1 and s+2 are addressed from the record base, not from the advancing segment base; the 001CB950 doubleword is 0x20048BA199422040 (the old C sign-extended its low word to 0xFFFFFFFF99422040); the block headers are two word stores of zero, as in the original, not one quadword store; 001CB950 is called with three arguments (the original leaves $a3 unset, and neither 001CB950 nor 001CB5F0 reads it). Residual: register coloring of the segment-loop locals.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// GS/DMA packet builder over one 0xA060-byte record of the array at
// D_00275C18, selected by arg0's unsigned halfword +0xE (the product is
// 32-bit). For each of 6 segments s: acquires a 0x1A-qword block via
// func_001CB5F0(D_007635C0, 0, 0x1A), writes the header words 0, 0,
// 0x01000404, 0x6C188000, then for 8 rows copies one quadword each (24
// func_00102948 calls, in row order) from the +0x60 rows of the record's
// 0x200-byte segments s, s+1 and s+2 (all addressed from the record base)
// to block +0x10, +0x90 and +0x110 (+ row * 0x10). Segment 5 gets 0x14000000
// at +0x190, segments 0-4 get 0x17000000; +0x194/+0x198/+0x19C are zeroed.
//
// Then a 5-qword block (tag 0x6C040000) whose payload is copy_qw4'd from
// D_70003AC0, and a 9-qword block (tag 0x6C0803F8): +0x10/14/18 = record
// +0x48/4C/50, +0x1C = 0 (stored before the next call), +0x20 = copy of
// record +0x10, +0x30/34/38/3C = record +0x3C/38/40/44 (stored before the
// next call), +0x40 = copy of D_008105D0, +0x50 = doubleword
// 0x303E400000008010, +0x58 = doubleword 0x412, +0x60/+0x70/+0x80 = copies
// of D_00275670 +0xA0/+0x2220/+0x2230. Finally func_001CB950(D_007635C0, 0,
// 0x20048BA199422040), func_001CB6B0(D_007635C0, 0, 8, D_00275674 + 0x720)
// and func_001CB760(D_007635C0, 0, &D_00234FE0).
//
// Read order (original; it matters when a packet block overlaps the data it reads, so the C
// keeps it): the record base is computed ONCE at entry from D_00275C18 and the node's +0xE;
// in the 9-qword block, record +0x48 is read after the header stores, +0x4C/+0x50 after the
// +0x10/+0x14 stores, +0x10 (the +0x20 copy) after the +0x1C store, +0x3C after the +0x20
// copy call, +0x38/+0x40/+0x44 after the +0x30/+0x34/+0x38 stores; D_00275670 is
// re-read for each of the +0x60/+0x70/+0x80 copies (after the previous copy call), and
// D_00275674 is read after the 001CB950 call (whose allocation may move the render context's
// cursor word).
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern char *func_001CB5F0(void *a0, int a1, int a2);
extern unsigned char *func_001CB950(int a0, int a1, long a2);
extern void func_001CB6B0(int a0, int a1, int a2, int a3);
extern void func_001CB760(int a0, int a1, int a2);

extern char D_007635C0[64];
extern char *D_00275C18;
extern char D_70003AC0[];
extern char D_008105D0[];
extern char *D_00275670;
extern char *D_00275674;
extern char D_00234FE0[];

void func_001E9280(char *arg0) {
    char *orig;
    char *base;
    char *seg;
    char *seg1;
    char *seg2;
    char *blk;
    int off;
    int row;
    int seg_idx;

    orig = D_00275C18 + *(unsigned short *)(arg0 + 0xE) * 0xA060;
    base = orig;
    for (seg_idx = 0; seg_idx < 6; seg_idx++) {
        blk = func_001CB5F0(D_007635C0, 0, 0x1A);
        *(int *)(blk + 0x0) = 0;
        *(int *)(blk + 0x4) = 0;
        *(int *)(blk + 0x8) = 0x01000404;
        *(int *)(blk + 0xC) = 0x6C188000;
        seg1 = orig + ((seg_idx + 1) << 9);
        seg2 = orig + ((seg_idx + 2) << 9);
        off = 0;
        seg = base;
        row = 0;
        do {
            func_00102948(blk + off + 0x10, seg + 0x60);
            func_00102948(blk + ((row + 8) << 4) + 0x10, seg1 + 0x60);
            func_00102948(blk + ((row + 0x10) << 4) + 0x10, seg2 + 0x60);
            row += 1;
            seg += 0x10;
            off += 0x10;
            seg1 += 0x10;
            seg2 += 0x10;
        } while (row < 8);
        if (seg_idx == 5) {
            *(int *)(blk + 0x190) = 0x14000000;
        } else {
            *(int *)(blk + 0x190) = 0x17000000;
        }
        *(int *)(blk + 0x194) = 0;
        *(int *)(blk + 0x198) = 0;
        *(int *)(blk + 0x19C) = 0;
        base += 0x200;
    }

    blk = func_001CB5F0(D_007635C0, 0, 5);
    *(int *)(blk + 0x0) = 0;
    *(int *)(blk + 0x4) = 0;
    *(int *)(blk + 0x8) = 0x01000404;
    *(int *)(blk + 0xC) = 0x6C040000;
    copy_qw4(blk + 0x10, D_70003AC0);

    blk = func_001CB5F0(D_007635C0, 0, 9);
    *(int *)(blk + 0x0) = 0;
    *(int *)(blk + 0x4) = 0;
    *(int *)(blk + 0x8) = 0x01000404;
    *(int *)(blk + 0xC) = 0x6C0803F8;
    *(float *)(blk + 0x10) = *(float *)(orig + 0x48);
    *(float *)(blk + 0x14) = *(float *)(orig + 0x4C);
    *(float *)(blk + 0x18) = *(float *)(orig + 0x50);
    *(int *)(blk + 0x1C) = 0;
    func_00102948(blk + 0x20, orig + 0x10);
    *(float *)(blk + 0x30) = *(float *)(orig + 0x3C);
    *(float *)(blk + 0x34) = *(float *)(orig + 0x38);
    *(float *)(blk + 0x38) = *(float *)(orig + 0x40);
    *(float *)(blk + 0x3C) = *(float *)(orig + 0x44);
    func_00102948(blk + 0x40, D_008105D0);
    *(long *)(blk + 0x50) = (long)(0x8010 | (0x303E4000LL << 32));
    *(long *)(blk + 0x58) = 0x412;
    func_00102948(blk + 0x60, D_00275670 + 0xA0);
    func_00102948(blk + 0x70, D_00275670 + 0x2220);
    func_00102948(blk + 0x80, D_00275670 + 0x2230);

    func_001CB950((int)D_007635C0, 0, (long)0x99422040U | ((long)0x20048BA1 << 32));
    func_001CB6B0((int)D_007635C0, 0, 8, (int)(D_00275674 + 0x720));
    func_001CB760((int)D_007635C0, 0, (int)D_00234FE0);
}
