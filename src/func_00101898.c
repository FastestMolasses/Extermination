// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Written in the SDK libgraph shape: GS registers as bit-field structs, the
// GIF tags cleared as one 128-bit store and then filled field by field.
// DISPFB is declared with 32-bit bit-fields (the FBP store then keeps the
// short-to-int narrowing the original has for that one field).
//
// SDK libgraph: sets up a default double-buffer record (two display
// environments, two drawing-environment pairs each preceded by a GIF tag,
// and two optional clear blocks) for a psm/w/h frame buffer with the given
// Z test and Z format:
//  - both display environments via func_001002E0 (offset 0, 0);
//  - each buffer's two drawing environments via func_001006D8 and
//    func_00101630;
//  - if clear is set, each buffer's clear block via func_001008C0 (centred
//    w x h rectangle at 2048 - w/2, 2048 - h/2, colour 0, depth 0);
//  - the GIF tags: NLOOP 22 with the clear block, 16 without; EOP, one
//    register (A+D);
//  - when the video mode (func_00100268) has inter == 1 and ffmode == 1
//    (interlaced, SMODE2 FFMD set) or inter == 0, display 1 and the first
//    buffer's two drawing environments get FBP = size >> 1, size being
//    func_00100610(psm, w, h); otherwise every FBP stays as the env setters
//    left it. The video-mode test reads inter and ffmode as one masked 64-bit
//    compare, as in func_00100610.

typedef unsigned long u_long;
typedef unsigned int u_long128 __attribute__((mode(TI)));

typedef struct {
    short inter;
    short omode;
    short ffmode;
} GParam;

typedef struct {
    unsigned int FBP:9; unsigned int FBW:6; unsigned int PSM:5; unsigned int p0:12;
    unsigned int DBX:11; unsigned int DBY:11; unsigned int p1:10;
} sceGsDispfb;

typedef struct {
    u_long FBP:9; u_long p0:7; u_long FBW:6; u_long p1:2; u_long PSM:6;
    u_long p2:2; u_long FBMSK:32;
} sceGsFrame;

typedef struct {
    u_long NLOOP:15; u_long EOP:1; u_long pad16:16; u_long id:14;
    u_long PRE:1; u_long PRIM:11; u_long FLG:2; u_long NREG:4;
    u_long REGS0:4; u_long REGS1:4; u_long REGS2:4; u_long REGS3:4;
    u_long REGS4:4; u_long REGS5:4; u_long REGS6:4; u_long REGS7:4;
    u_long REGS8:4; u_long REGS9:4; u_long REGS10:4; u_long REGS11:4;
    u_long REGS12:4; u_long REGS13:4; u_long REGS14:4; u_long REGS15:4;
} sceGifTag;

typedef struct {
    u_long pmode;
    u_long smode2;
    sceGsDispfb dispfb;
    u_long display;
    u_long bgcolor;
} DispEnv;

typedef struct {
    sceGsFrame frame;
    u_long rest[15];
} DrawEnv;

typedef struct {
    u_long regs[12];
} ClearEnv;

typedef struct {
    DispEnv disp[2];
    sceGifTag giftag0;
    DrawEnv draw01;
    DrawEnv draw02;
    ClearEnv clear0;
    sceGifTag giftag1;
    DrawEnv draw11;
    DrawEnv draw12;
    ClearEnv clear1;
} DBuffDc;

extern GParam *func_00100268(void);
extern void func_001002E0(DispEnv *disp, short psm, short w, short h, short dx, short dy);
extern int func_001006D8(DrawEnv *draw, short psm, short w, short h, short ztest, short zpsm);
extern int func_00101630(DrawEnv *draw, short psm, short w, short h, short ztest, short zpsm);
extern int func_001008C0(ClearEnv *clear, short ztest, short x, short y, short w, short h,
                         unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                         unsigned int z);
extern short func_00100610(short psm, short w, short h);

void func_00101898(DBuffDc *db, short psm, short w, short h, short ztest, short zpsm,
                   short clear)
{
    GParam *gp;
    short fbp;

    gp = func_00100268();
    func_001002E0(&db->disp[0], psm, w, h, 0, 0);
    func_001002E0(&db->disp[1], psm, w, h, 0, 0);
    func_001006D8(&db->draw01, psm, w, h, ztest, zpsm);
    func_00101630(&db->draw02, psm, w, h, ztest, zpsm);
    func_001006D8(&db->draw11, psm, w, h, ztest, zpsm);
    func_00101630(&db->draw12, psm, w, h, ztest, zpsm);
    if (clear) {
        func_001008C0(&db->clear0, ztest, 2048 - (w >> 1), 2048 - (h >> 1), w, h,
                      0, 0, 0, 0, 0);
        func_001008C0(&db->clear1, ztest, 2048 - (w >> 1), 2048 - (h >> 1), w, h,
                      0, 0, 0, 0, 0);
    }

    *(u_long128 *)&db->giftag0 = 0;
    db->giftag0.NLOOP = clear ? 22 : 16;
    db->giftag0.EOP = 1;
    db->giftag0.NREG = 1;
    db->giftag0.REGS0 = 0xE;
    *(u_long128 *)&db->giftag1 = 0;
    db->giftag1.NLOOP = clear ? 22 : 16;
    db->giftag1.EOP = 1;
    db->giftag1.NREG = 1;
    db->giftag1.REGS0 = 0xE;

    fbp = func_00100610(psm, w, h);
    if ((*(long *)gp & 0xFFFF0000FFFFL) == 0x100000001L || gp->inter == 0) {
        db->disp[1].dispfb.FBP = fbp >> 1;
        db->draw01.frame.FBP = fbp >> 1;
        db->draw02.frame.FBP = fbp >> 1;
    }
}
