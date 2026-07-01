// NEARMISS func_001E9280  (vram 0x001E9280, 0x2F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.76% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Best build is mwcc233 (84.76%) vs mwcc991202 (75.04%). Full logic recovered: a GS/DMA display-list builder for a 6-segment tendril mesh (per-bone data at D_00275C18 + bone_idx*0xA060). Builds 6 GIFtag blocks via func_001CB5F0 (calling convention cross-checked against the already-matched siblings ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// GS/DMA display-list builder for a 6-segment tendril mesh (per-bone array at
// D_00275C18 + bone_idx*0xA060). For each of 6 segments: acquires a 0x1A-qword
// GIFtag block via func_001CB5F0(D_007635C0, 0, 0x1A), fills the standard
// GIFtag header (TOP=0x01000404, TAG=0x6C188000: PACKED, 8 regs, unclip prim),
// then copies 3 quadwords of vertex data per each of 8 rows (24 copy_qw calls)
// from the segment's own +0x60, the next segment's +0x60, and the segment
//-after-next's +0x60, writing into consecutive 0x10-byte destination slots.
// Segment 5 (the last) gets a smaller draw-kick word (0x14000000 vs 0x17000000
// for segments 0-4) at +0x190; +0x194/+0x198/+0x19C are zeroed (draw-state
// tail). After the 6-segment loop, two more GIFtag blocks are appended: a
// 5-qword block (tag 0x6C040000) whose payload is copy_qw4'd from the fixed
// scratch D_70003AC0, and a 9-qword block (tag 0x6C0803F8) carrying the
// segment-0 sub-object's origin/basis (+0x48/4C/50 -> xyz, +0x3C/38/40/44 ->
// quat via copy_qw), a fixed AD/GIF-reg pair (0x8010 | 0x303E4000<<32,
// 0x412), and 3 texture/palette copy_qw's sourced off D_00275670. Finally,
// three fixed-format command appends close out the list: func_001CB950 (a
// tag-3 5-qword block carrying a packed AD command), func_001CB6B0 (arg3 =
// D_00275674 + 0x720), and func_001CB760 (arg2 = &D_00234FE0).
typedef unsigned __int128 uint128;
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern char *func_001CB5F0(void *a0, int a1, int a2);
extern unsigned char *func_001CB950(int a0, int a1, long a2, int a3);
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
    char *dst;
    int row;
    int seg_idx;

    orig = D_00275C18 + *(unsigned short *)(arg0 + 0xE) * 0xA060;
    base = orig;
    for (seg_idx = 0; seg_idx < 6; seg_idx++) {
        blk = func_001CB5F0(D_007635C0, 0, 0x1A);
        *(uint128 *)(blk + 0x0) = 0;
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
    *(uint128 *)(blk + 0x0) = 0;
    *(int *)(blk + 0x8) = 0x01000404;
    *(int *)(blk + 0xC) = 0x6C040000;
    copy_qw4(blk + 0x10, D_70003AC0);

    blk = func_001CB5F0(D_007635C0, 0, 9);
    *(uint128 *)(blk + 0x0) = 0;
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

    func_001CB950((int)D_007635C0, 0, (long)(0x9942 << 16 | 0x2040) | ((long)0x20048BA1 << 32), 0);
    func_001CB6B0((int)D_007635C0, 0, 8, (int)(D_00275674 + 0x720));
    func_001CB760((int)D_007635C0, 0, (int)D_00234FE0);
}
