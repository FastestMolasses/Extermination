// NEARMISS func_001F0720  (vram 0x001F0720, 0x334 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 86.25 with mwcc233 -O4,p -sdatathreshold 4 (o991 69.09, o24 86.25). Dispatch, all 7 preset arms, the age loop's beql, the `do { } while (k <= 0)` shape, the quadword copy loop and the entire four-packet DMA tail are structurally correct and byte-identical in ~150 of 210 instructions. The wall is ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: age and then draw one whole particle-effect lane.  `n` selects one of
// 7 effect presets via jtbl_0026E9E0 (ids 0..6); any other id returns immediately.
// This is the render-side companion of func_001F0460, and shares its storage:
// the lane base is D_0028F700 + 0x4DBEC0 + n*3072, holding 32 slots of 96 bytes.
// Each preset supplies a lifetime `count` (whose *60 is the slot countdown domain),
// a `scale` used to rescale the slot's float at +0x4C, and `pal`, an index into the
// 16-byte colour records at D_00259CD0:
//   0: count 1, scale 64, pal 0     4: count 2, scale 80, pal 8
//   1: count 2, scale 64, pal 4     5: count 1, scale 64, pal 0xC
//   2: count 2, scale 64, pal 4     6: count 1, scale 64, pal 0xC
//   3: count 2, scale 64, pal 8
// Age pass: for all 32 slots, the countdown at +0x58 is decremented; once it drops
// below count*60 the normalised remaining life is written to the scratchpad float at
// 0x70003A20 and scale*that is stored to the slot's +0x4C; an already-expired slot
// is clamped to 0.
// Draw pass: four DMA/GIF packets are opened on channel 0 of the D_007635C0 chain
// with func_001CB5F0(chain, 0, qwords) and filled in place --
//   1 qw : mode words 0x11000000 / 0x14000000 / 0x11000000
//   0xC1 : tag 0x6C000000 | (0xC0 << 16) | addr, followed by a straight quadword
//          copy of the whole 0xC00-byte lane (this is the k loop, which the original
//          writes as a one-iteration `do { } while (k <= 0)`)
//   5    : tag 0x6C040000 | 0xE  + the preset's 4-quadword colour record
//   0xA  : tag 0x6C090000 + the transform from func_001CD370(2), the scratchpad
//          matrix at D_70003AC0, and the camera block at D_00275670 + 0xA0
// then the chain is closed with func_001CB760(chain, 0, D_00233290) and kicked with
// func_001CB900(chain, 0, 1).

typedef int s128 __attribute__((mode(TI)));

extern void  copy_qw4(void *dst, void *src);
extern void  func_00102948(void *dst, void *src);
extern char *func_001CB5F0(void *dma, int chan, int qwords);
extern void  func_001CB760(void *dma, int chan, void *cb);
extern void  func_001CB900(void *dma, int chan, int flag);
extern char *func_001CD370(int id);

extern char  D_007635C0[];
extern char  D_0028F700[];
extern char  D_00259CD0[];
extern char  D_00233290[];
extern char  D_70003AC0[];
extern char *D_00275670;

void func_001F0720(int n)
{
    char *lane;
    char *slot;
    char *pkt;
    char *mtx;
    s128 *src;
    s128 *dst;
    float scale;
    float ftotal;
    float f;
    int total;
    int count;
    int pal;
    int i;
    int k;
    int t;
    int cnt;
    int qwc;
    int addr;
    int addr2;
    int addr3;

    addr2 = 0xE;
    addr = 0x20;
    qwc = 0xC0;
    addr3 = 0;

    switch (n) {
    case 0:
        count = 1;
        scale = 64.0f;
        pal = 0;
        break;
    case 1:
        count = 2;
        scale = 64.0f;
        pal = 4;
        break;
    case 2:
        count = 2;
        scale = 64.0f;
        pal = 4;
        break;
    case 3:
        count = 2;
        scale = 64.0f;
        pal = 8;
        break;
    case 4:
        count = 2;
        scale = 80.0f;
        pal = 8;
        break;
    case 5:
        count = 1;
        scale = 64.0f;
        pal = 0xC;
        break;
    case 6:
        count = 1;
        scale = 64.0f;
        pal = 0xC;
        break;
    default:
        return;
    }

    lane = D_0028F700 + ((n * 3) << 10) + 0x4DBEC0;
    total = count * 60;
    ftotal = (float)total;
    slot = lane;
    i = 0;
    do {
        t = *(int *)(slot + 0x58);
        if (t > 0) {
            *(int *)(slot + 0x58) = t - 1;
            t = *(int *)(slot + 0x58);
            if (t < total) {
                f = (float)t / ftotal;
                *(volatile float *)0x70003A20 = f;
                *(float *)(slot + 0x4C) = scale * f;
            }
        } else {
            *(int *)(slot + 0x58) = 0;
        }
        i += 1;
        slot += 0x60;
    } while (i < 0x20);

    pkt = func_001CB5F0(D_007635C0, 0, 1);
    *(s128 *)pkt = 0;
    *(int *)pkt = 0x11000000;
    *(int *)(pkt + 4) = 0x14000000;
    *(int *)(pkt + 8) = 0x11000000;

    k = 0;
    do {
        pkt = func_001CB5F0(D_007635C0, 0, 0xC1);
        *(s128 *)pkt = 0;
        *(int *)(pkt + 8) = 0x01000101;
        *(int *)(pkt + 0xC) = (addr | (qwc << 16)) | 0x6C000000;
        dst = (s128 *)(pkt + 0x10);
        cnt = qwc;
        src = (s128 *)lane;
        do {
            *dst = *src;
            cnt -= 1;
            dst += 1;
            src += 1;
        } while (cnt != 0);
        k += 1;
        lane += 0xC00;
        addr += 0xC0;
    } while (k <= 0);

    pkt = func_001CB5F0(D_007635C0, 0, 5);
    *(s128 *)pkt = 0;
    *(int *)(pkt + 8) = 0x01000101;
    *(int *)(pkt + 0xC) = addr2 | 0x6C040000;
    copy_qw4(pkt + 0x10, D_00259CD0 + pal * 16);

    mtx = func_001CD370(2);
    pkt = func_001CB5F0(D_007635C0, 0, 0xA);
    *(s128 *)pkt = 0;
    *(int *)(pkt + 8) = 0x01000101;
    *(int *)(pkt + 0xC) = addr3 | 0x6C090000;
    copy_qw4(pkt + 0x10, mtx);
    copy_qw4(pkt + 0x50, D_70003AC0);
    func_00102948(pkt + 0x90, D_00275670 + 0xA0);
    func_001CB760(D_007635C0, 0, D_00233290);
    func_001CB900(D_007635C0, 0, 1);
}
