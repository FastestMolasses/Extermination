// NEARMISS func_001E9E60  (vram 0x001E9E60, 0x3A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Genuine FPU-MAC pipeline (mula.s/madd.s) computing a per-axis lerp result=base+(target-base)*blend at +0x20..0x2C -- no scalar C reproduces mwcc's paired multiply-accumulate scheduling for this (VU0/FPU-MAC-class dead wall, same as sibling func_001E9280/func_001CB2C0). Remaining residual after th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// GS/DMA display-list builder for a 6-segment tendril mesh, sibling of
// func_001E9280 but selecting the per-level bone array by arg1 (D_00275C1C +
// arg1*0xA060) instead of reading a bone index out of arg0. For each of 6
// segments: acquires a 0x1A-qword GIFtag block via func_001CB5F0(D_007635C0,
// 0, 0x1A), fills the standard GIFtag header (TOP=0x01000404,
// TAG=0x6C188000: PACKED, 8 regs, unclip prim), then copies 3 quadwords of
// vertex data per each of 8 rows (24 copy_qw calls) from the segment's own
// +0x60, the next segment's +0x60, and the segment-after-next's +0x60.
// Segment 5 (the last) gets a smaller draw-kick word (0x14000000 vs
// 0x17000000) at +0x190; +0x194/+0x198/+0x19C are zeroed.
//
// After the 6-segment loop: a 5-qword block (tag 0x6C040000) copy_qw4'd from
// the fixed scratch D_70003AC0; then a 9-qword block (tag 0x6C0803F8)
// carrying the segment-0 sub-object's origin (+0x48/4C/50 -> xyz) plus a W
// built from arg0's +0xB4 field (60.0f + arg0[0xB4]). The block's +0x20..0x2C
// quad is a per-axis lerp: blend = D_0026E9B0/B4/B8 (a 3-vector), base =
// segment's own +0x10/14/18 (a 3-vector), delta = arg0's +0x80/84/88 minus
// base (delta = arg0_vec - base_vec), result = base + delta*blend (i.e. the
// mula.s/madd.s FPU-MAC pipeline computing base + (arg0_vec-base)*blend per
// axis). +0x1C's W word is (segment's own +0x1C) * arg0's +0x8C. Then the
// quat (+0x3C/38/40/44 -> +0x30/34/38/3C), a fixed AD/GIF-reg pair
// (0x8010 | 0x303E4000<<32, 0x412), and 3 texture/palette copy_qw's sourced
// off D_00275670. Finally three fixed-format command appends close out the
// list: func_001CB950 (a tag-3 5-qword block whose packed AD command depends
// on whether the segment-0 object's +0x5C byte is 1 — two distinct constant
// pairs), func_001CB6B0 (arg3 = D_00275674 + 0x720), and func_001CB760
// (arg2 = &D_002345E0).
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern char *func_001CB5F0(void *a0, int a1, int a2);
extern unsigned char *func_001CB950(int a0, int a1, long a2, int a3);
extern void func_001CB6B0(int a0, int a1, int a2, int a3);
extern void func_001CB760(int a0, int a1, int a2);

extern char D_007635C0[64];
extern char *D_00275C1C;
extern char D_70003AC0[];
extern char D_008105D0[];
extern char *D_00275670;
extern char *D_00275674;
extern char D_002345E0[];
extern float D_0026E9B0;
extern float D_0026E9B4;
extern float D_0026E9B8;

void func_001E9E60(char *arg0, int arg1) {
    char *orig;
    char *base;
    char *seg;
    char *seg1;
    char *seg2;
    char *blk;
    char *dst;
    int row;
    int seg_idx;
    float spB0;
    float spB4;
    float spB8;
    float spBC;

    orig = D_00275C1C + arg1 * 0xA060;
    base = orig;
    for (seg_idx = 0; seg_idx < 6; seg_idx++) {
        blk = func_001CB5F0(D_007635C0, 0, 0x1A);
        *(int *)(blk + 0x0) = 0;
        *(int *)(blk + 0x4) = 0;
        *(int *)(blk + 0x8) = 0x01000404;
        *(int *)(blk + 0xC) = 0x6C188000;
        seg1 = base + ((seg_idx + 1) << 9);
        seg2 = base + ((seg_idx + 2) << 9);
        dst = blk + 0x10;
        seg = base;
        row = 0;
        do {
            func_00102948(dst, seg + 0x60);
            func_00102948(dst + 0x80, seg1 + 0x60);
            func_00102948(dst + 0x100, seg2 + 0x60);
            row += 1;
            seg += 0x10;
            dst += 0x10;
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
    *(float *)(blk + 0x1C) = 60.0f + *(float *)(arg0 + 0xB4);

    spB0 = *(float *)(orig + 0x10) + (*(float *)(arg0 + 0x80) - *(float *)(orig + 0x10)) * D_0026E9B0;
    spB4 = *(float *)(orig + 0x14) + (*(float *)(arg0 + 0x84) - *(float *)(orig + 0x14)) * D_0026E9B4;
    spB8 = *(float *)(orig + 0x18) + (*(float *)(arg0 + 0x88) - *(float *)(orig + 0x18)) * D_0026E9B8;
    spBC = *(float *)(orig + 0x1C) * *(float *)(arg0 + 0x8C);
    func_00102948(blk + 0x20, &spB0);

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

    if (*(unsigned char *)(orig + 0x5C) != 1) {
        func_001CB950((int)D_007635C0, 0, (long)0x55422242 | ((long)0x20048CC1 << 32), 0);
    } else {
        func_001CB950((int)D_007635C0, 0, (long)0x55422256 | ((long)0x20048E41 << 32), 0);
    }
    func_001CB6B0((int)D_007635C0, 0, 8, (int)(D_00275674 + 0x720));
    func_001CB760((int)D_007635C0, 0, (int)D_002345E0);
}
