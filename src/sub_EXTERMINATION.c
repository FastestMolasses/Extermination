// NEARMISS sub_EXTERMINATION  (vram 0xXTERMINATION, 0xAA8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// REGISTER-ALLOCATION-ORDER wall (park / permuter candidate). Instruction COUNT matches exactly (720 vs 720) and the emitted shape is recovered instruction for instruction — including the `movz a1,tplA,i` template select in bank A, the `sq` quadword arena clear, the two jr-table dispatches (jtbl_00...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: one-shot boot-time builder for the whole GS/DMA display-list arena.
//   * clears the 0x4F7AC-quadword packet arena at D_0028F700 with the 128-bit
//     pattern held at D_0026E3F0, then brings up the video/render subsystems
//     (func_0021B860, the func_001D2830 group enables, func_001D25F0(480.0f),
//     func_001DEDE0);
//   * func_00101898 fills a video-mode descriptor on the stack (0x200 x 0xE0,
//     mode 2/0x31, interlace 1); its +0x70 halfword and +0x73 byte are the
//     FRAME/scissor fields folded into every GS packet built below;
//   * then writes seven banks of GIFtag + AD-register packets into the arena
//     pointed to by the gp-relative D_00275674:
//       +0x020 x2 (stride 0x190) main draw env, body copied from the two stack
//               templates; +0x340 x2 (stride 0x30); +0x3A0 x2 (stride 0x80);
//       +0x4A0 x4 (stride 0x40) TEST/ALPHA variants; +0x5A0 x4 (stride 0x40)
//               field/FRAME variants; +0x6A0 x10 (stride 0x80) blend presets;
//       +0xBA0 4x10 (strides 0x5A0 / 0x90) the per-pass blend preset matrix,
//               whose GS mode word carries pass index `mode` in bits 17+.
//   * finally zeroes the arena header, hands it to func_0021B970 with the two
//     timing constants, resets the fog/clear colour (func_0021BA80) and
//     duplicates the 0x30-byte matrix at D_00275670+0xA0 into +0xC0 and +0x100.
//
// NEARMISS 91.38% (mwcc 2.3.3; 81.67% on the pinned 2.3.1.01 build).  Instruction
// COUNT matches exactly (720 = 720) and the shape is recovered instruction for
// instruction -- including the `movz` template select in bank A, the `sq` arena
// clear, every GIFtag/AD constant and all seven loop strides.  384 of the 720
// instructions differ only in which register they name: CodeWarrior and mwcc pick
// different saved/temporary numbers for the per-bank (counter, offset, packet
// pointer) triples (e.g. bank A target s4=packet/s3=index/s0=offset vs mwcc
// s1/s0/s4).  This is the register-allocation-ORDER wall -- declaration and
// first-use ordering already match, mwcc just numbers the registers differently --
// so it is a permuter job, not a source-idiom one.  The only non-register
// residuals left are the 3-instruction address formation for D_0026E3F0 (CW forms
// lui/addiu then `ld 0(reg)`; mwcc folds it to lui/%lo) and two scheduling swaps in
// banks E and F.
extern void block_copy(void *dst, void *src, int len);
extern void func_00101898(void *env, int a, int w, int h, int mode, int b, int c);
extern void func_001CB5C0(void *p);
extern void func_001D25F0(float f);
extern void func_001D2830(int group, int enable);
extern void func_001DEDE0(void);
extern void func_0021B860(void);
extern void func_0021B970(float a, float b);
extern void func_0021BA80(int r, int g, int b);

typedef int u128 __attribute__((mode(TI)));

extern long long D_0026E3F0[4];
extern char *D_00275670;
extern char *D_00275674;
extern char D_0028F700[];
extern int D_007635C0[];

void sub_EXTERMINATION(void) {
    char fb[0x330];             /* stack scratch: video-mode desc + packet templates */
    char *p;
    char *q;
    char *tplA;
    char *tplB;
    u128 pat;
    unsigned int i;
    int off;
    int koff;
    int k;
    int mode;
    int field;
    long long modebits;
    long long ad1;

    i = 0;
    pat = (u128)D_0026E3F0[0];
    q = D_0028F700;
    do {
        i += 1;
        *(u128 *)q = pat;
        q += 0x10;
    } while (i < 0x4F7AC);

    func_0021B860();
    func_001D2830(0, 1);
    func_001D25F0(480.0f);
    func_001D2830(2, 0);
    func_001D2830(9, 0);
    func_001DEDE0();
    func_001D2830(1, 1);
    func_001D2830(0x24, 0);
    func_001D2830(0x25, 0);
    func_001D2830(8, 0);
    func_00101898(fb, 0, 0x200, 0xE0, 2, 0x31, 1);

    *(long long *)(fb + 0x180) = (long long)0x8000 << 0x10;   /* sp+0x200 */
    *(long long *)(fb + 0x2F0) = (long long)0x8000 << 0x10;   /* sp+0x370 */

    /* bank A: two 0x190-stride main draw environments */
    tplA = fb + 0x50;
    tplB = fb + 0x1C0;
    i = 0;
    off = 0;
    do {
        p = D_00275674 + off + 0x20;
        block_copy(p + 0x10, (i == 0) ? tplA : tplB, 0x110);
        *(int *)(p + 0x00) = 0;
        *(int *)(p + 0x04) = 0;
        *(int *)(p + 0x08) = 0x11000000;
        *(int *)(p + 0x0C) = 0x50000018;
        *(long long *)(p + 0x10) = ((long long)0x10000000 << 0x20) | 0x8017;
        *(long long *)(p + 0x18) = 0xE;
        *(long long *)(p + 0x120) = 0;
        *(long long *)(p + 0x128) = 8;
        *(long long *)(p + 0x130) = 1;
        *(long long *)(p + 0x138) = 0x46;
        *(long long *)(p + 0x140) = 0;
        *(long long *)(p + 0x148) = 0x4A;
        *(long long *)(p + 0x150) = 0;
        *(long long *)(p + 0x158) = 0x49;
        *(long long *)(p + 0x160) = 0;
        *(long long *)(p + 0x168) = 0x22;
        *(long long *)(p + 0x170) = 0x60;
        *(long long *)(p + 0x178) = 0x14;
        *(long long *)(p + 0x180) = (long long)1 << 0x20;
        i += 1;
        *(long long *)(p + 0x188) = 0x3B;
        off += 0x190;
    } while (i < 2);

    /* bank B: two 0x30-stride single-register packets at +0x340 */
    i = 0;
    off = 0;
    do {
        i += 1;
        p = D_00275674 + off;
        *(int *)(p + 0x340) = 0;
        *(int *)(p + 0x344) = 0;
        *(int *)(p + 0x348) = 0x11000000;
        *(int *)(p + 0x34C) = 0x50000002;
        *(long long *)(p + 0x350) = ((long long)0x10000000 << 0x20) | 0x8001;
        *(long long *)(p + 0x358) = 0xE;
        *(long long *)(p + 0x360) = 0;
        off += 0x30;
        *(long long *)(p + 0x368) = 0x3D;
    } while (i < 2);

    /* bank C: two 0x80-stride packets at +0x3A0 */
    i = 0;
    off = 0;
    do {
        p = D_00275674 + off + 0x3A0;
        block_copy(p + 0x20, fb + 0x160, 0x60);
        *(int *)(p + 0x00) = 0;
        *(int *)(p + 0x04) = 0;
        *(int *)(p + 0x08) = 0x11000000;
        *(int *)(p + 0x0C) = 0x50000007;
        *(long long *)(p + 0x10) = ((long long)0x10000000 << 0x20) | 0x8006;
        *(long long *)(p + 0x18) = 0xE;
        switch (i) {
        case 0:
            *(long long *)(p + 0x20) = 0x32001;
            break;
        case 1:
            *(long long *)(p + 0x20) = 0x30000;
            break;
        }
        i += 1;
        off += 0x80;
    } while (i < 2);

    /* bank D: four 0x40-stride TEST-register packets at +0x4A0 */
    i = 0;
    off = 0;
    do {
        *(int *)(D_00275674 + off + 0x4A0) = 0;
        *(int *)(D_00275674 + off + 0x4A4) = 0;
        *(int *)(D_00275674 + off + 0x4A8) = 0x11000000;
        *(int *)(D_00275674 + off + 0x4AC) = 0x50000003;
        *(long long *)(D_00275674 + off + 0x4B0) = ((long long)0x10000000 << 0x20) | 0x8002;
        *(long long *)(D_00275674 + off + 0x4B8) = 0xE;
        switch (i) {
        case 0:
            *(long long *)(D_00275674 + off + 0x4D0) = 5;
            break;
        case 1:
            *(long long *)(D_00275674 + off + 0x4D0) = 0;
            break;
        case 2:
            *(long long *)(D_00275674 + off + 0x4D0) = ((long long)0x37C << 0x20) | 0x7FC00A;
            break;
        case 3:
            *(long long *)(D_00275674 + off + 0x4D0) = ((long long)0x3FC << 0x20) | 0x3FC00A;
            break;
        }
        i += 1;
        p = D_00275674 + off;
        off += 0x40;
        *(long long *)(p + 0x4D8) = 8;
    } while (i < 4);

    /* bank E: four 0x40-stride FRAME/field packets at +0x5A0 */
    i = 0;
    off = 0;
    do {
        p = D_00275674 + off;
        *(int *)(p + 0x5A0) = 0x11000000;
        *(int *)(p + 0x5A4) = 0;
        *(int *)(p + 0x5A8) = 0;
        *(int *)(p + 0x5AC) = 0x50000003;
        *(long long *)(p + 0x5B0) = ((long long)0x10000000 << 0x20) | 0x8002;
        *(long long *)(p + 0x5B8) = 0xE;
        p += 0x5A0;
        switch (i) {
        case 1:
            *(long long *)(p + 0x20) = 0x5000D;
            *(long long *)(p + 0x30) = ((long long)*(unsigned short *)(fb + 0x70) & 0x1FF) |
                                       (((long long)*(unsigned char *)(fb + 0x73) & 0xF) << 24);
            break;
        case 0:
            *(long long *)(p + 0x20) = 0x3000D;
            *(long long *)(p + 0x30) = (((long long)*(unsigned short *)(fb + 0x70) & 0x1FF) |
                                        (((long long)*(unsigned char *)(fb + 0x73) & 0xF) << 24)) |
                                       ((long long)1 << 0x20);
            break;
        case 2:
            *(long long *)(p + 0x20) = 0x5000D;
            *(long long *)(p + 0x30) = (((long long)*(unsigned short *)(fb + 0x70) & 0x1FF) |
                                        (((long long)*(unsigned char *)(fb + 0x73) & 0xF) << 24)) |
                                       ((long long)1 << 0x20);
            break;
        case 3:
            *(long long *)(p + 0x20) = 0x3000D;
            *(long long *)(p + 0x30) = ((long long)*(unsigned short *)(fb + 0x70) & 0x1FF) |
                                       (((long long)*(unsigned char *)(fb + 0x73) & 0xF) << 24);
            break;
        }
        *(long long *)(p + 0x28) = 0x47;
        i += 1;
        *(long long *)(p + 0x38) = 0x4E;
        off += 0x40;
    } while (i < 4);

    /* bank F: ten 0x80-stride blend presets at +0x6A0 */
    i = 0;
    off = 0;
    do {
        p = D_00275674 + off;
        *(int *)(p + 0x6A0) = 0;
        *(int *)(p + 0x6A4) = 0;
        *(int *)(p + 0x6A8) = 0x11000000;
        *(int *)(p + 0x6AC) = 0x50000007;
        *(long long *)(p + 0x6B0) = ((long long)0x10000000 << 0x20) | 0x8006;
        *(long long *)(p + 0x6B8) = 0xE;
        *(long long *)(p + 0x6C0) = 0x17E;
        *(long long *)(p + 0x6C8) = 0;
        *(long long *)(p + 0x6D0) = 0x60;
        *(long long *)(p + 0x6D8) = 0x14;
        *(long long *)(p + 0x700) = 0;
        *(long long *)(p + 0x708) = 8;
        *(long long *)(p + 0x710) = 1;
        *(long long *)(p + 0x718) = 0x46;
        *(long long *)(p + 0x6E8) = 0x47;
        q = p + 0x6A0;
        *(long long *)(p + 0x6F8) = 0x42;
        switch (i) {
        case 0:
            *(long long *)(q + 0x40) = 0x5000D;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0xA8;
            break;
        case 1:
            *(long long *)(q + 0x40) = 0x53001;
            *(long long *)(q + 0x50) = 0x44;
            break;
        case 2:
            *(long long *)(q + 0x40) = 0x53001;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0x68;
            break;
        case 3:
            *(long long *)(q + 0x40) = 0x5000D;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0x62;
            break;
        case 4:
            *(long long *)(q + 0x40) = 0x53001;
            *(long long *)(q + 0x50) = 0x49;
            break;
        case 5:
            *(long long *)(q + 0x40) = 0x5C00D;
            *(long long *)(q + 0x50) = 0x49;
            break;
        case 6:
            *(long long *)(q + 0x40) = 0x5C00D;
            *(long long *)(q + 0x50) = 0x44;
            break;
        case 7:
            *(long long *)(q + 0x40) = 0x50003;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0xA8;
            break;
        case 8:
            *(long long *)(q + 0x40) = 0x52001;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0xA8;
            break;
        case 9:
            *(long long *)(q + 0x40) = 0x51001;
            *(long long *)(q + 0x50) = ((long long)0x80 << 0x20) | 0xA9;
            break;
        }
        i += 1;
        off += 0x80;
    } while (i < 0xA);

    /* bank G: 4 passes x 10 blend presets at +0xBA0 (strides 0x5A0 / 0x90) */
    k = 0;
    off = 0;
    do {
        switch (k) {
        case 1:
            mode = 2;
            field = 0;
            break;
        case 0:
            mode = 1;
            field = 1;
            break;
        case 2:
            field = 1;
            mode = 2;
            break;
        case 3:
            mode = 1;
            field = 0;
            break;
        }
        modebits = (long long)mode << 0x11;
        ad1 = modebits | 0x13001;
        i = 0;
        koff = 0;
        do {
            p = D_00275674 + off + koff;
            *(int *)(p + 0xBA0) = 0;
            *(int *)(p + 0xBA4) = 0;
            *(int *)(p + 0xBA8) = 0x11000000;
            *(int *)(p + 0xBAC) = 0x50000008;
            *(long long *)(p + 0xBB0) = ((long long)0x10000000 << 0x20) | 0x8007;
            *(long long *)(p + 0xBB8) = 0xE;
            *(long long *)(p + 0xBC0) = 0x100;
            *(long long *)(p + 0xBC8) = 0;
            *(long long *)(p + 0xBD0) = 0x60;
            *(long long *)(p + 0xBD8) = 0x14;
            *(long long *)(p + 0xBE8) = 0x47;
            *(long long *)(p + 0xBF0) = ((long long)field << 0x20) |
                                        (((long long)*(unsigned short *)(fb + 0x70) & 0x1FF) |
                                         (((long long)*(unsigned char *)(fb + 0x73) & 0xF) << 24));
            *(long long *)(p + 0xBF8) = 0x4E;
            *(long long *)(p + 0xC08) = 0x42;
            *(long long *)(p + 0xC10) = 0;
            *(long long *)(p + 0xC18) = 8;
            *(long long *)(p + 0xC20) = 1;
            q = p + 0xBA0;
            *(long long *)(p + 0xC28) = 0x46;
            switch (i) {
            case 0:
                *(long long *)(q + 0x40) = modebits | 0x1000D;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0xA8;
                break;
            case 1:
                *(long long *)(q + 0x40) = ad1;
                *(long long *)(q + 0x60) = 0x44;
                break;
            case 2:
                *(long long *)(q + 0x40) = ad1;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0x68;
                break;
            case 3:
                *(long long *)(q + 0x40) = ad1;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0x62;
                break;
            case 4:
                *(long long *)(q + 0x40) = ad1;
                *(long long *)(q + 0x60) = 0x49;
                break;
            case 5:
                *(long long *)(q + 0x40) = 0x5C00D;
                *(long long *)(q + 0x60) = 0x49;
                break;
            case 6:
                *(long long *)(q + 0x40) = modebits | 0x1C00D;
                *(long long *)(q + 0x60) = 0x44;
                break;
            case 7:
                *(long long *)(q + 0x40) = modebits | 0x10003;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0xA8;
                break;
            case 8:
                *(long long *)(q + 0x40) = modebits | 0x12001;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0xA8;
                break;
            case 9:
                *(long long *)(q + 0x40) = modebits | 0x11001;
                *(long long *)(q + 0x60) = ((long long)0x80 << 0x20) | 0xA9;
                break;
            }
            i += 1;
            koff += 0x90;
        } while ((int)i < 0xA);
        k += 1;
        off += 0x5A0;
    } while (k < 4);

    *(int *)(D_00275674 + 0x00) = 0;
    *(int *)(D_00275674 + 0x04) = 0;
    *(int *)(D_00275674 + 0x08) = 0;
    *(int *)(D_00275674 + 0x0C) = 0x11000000;
    *(int *)(D_00275674 + 0x10) = 0x70000000;
    *(int *)(D_00275674 + 0x14) = 0;
    func_0021B970(1000000.0f, 10000000.0f);
    func_0021BA80(0, 0, 0);
    block_copy(D_00275670 + 0x100, D_00275670 + 0xA0, 0x30);
    block_copy(D_00275670 + 0xC0, D_00275670 + 0xA0, 0x30);
    func_001CB5C0(D_007635C0);
}
