// NEARMISS func_001002E0  (vram 0x001002E0, 0x270 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.15% via ee-gcc 2.9-991111-01 (-O2) (was 65.22%). Improved in the matching wave
// 2026-10-27: typed GParam/DispEnv records, a block-local magh per arm, and the DISPLAY value
// OR'ed in the order DX | DH | MAGH | DY | DW (that order alone moved the OR tree onto the
// original's pairing). Remaining differences: register colouring only — the mode pointer and
// mode[0] copy sit one argument register lower than the original's, and the non-interlaced
// arms keep the (w + 2559) numerator in a different register, so the original's single shared
// numerator in the field-test delay slot is emitted twice here (two extra instructions).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: libgraph-style "set default display environment" (Sony SDK lowmem region):
// fills a five-word GS display block from the current video mode.
//
// func_00100268() returns the current video-mode descriptor:
//   inter  = interlace mode, omode = tv standard (2 = NTSC, 3 = PAL), ffmode = frame/field.
//
// The five 64-bit words written to `disp`:
//   pmode   = 0x66            (PMODE: EN2, CRTMD=1, MMOD, AMOD)
//   smode2  = 2 non-interlaced, 3 interlaced FFMD, 1 interlaced field
//   dispfb  = FBW ((w + 63) >> 6) at bit 9, PSM at bit 15
//   display = DX 0..11, DY 12..22, MAGH 23..26, DW 32..43, DH 44..54
//   bgcolor = 0
//
// MAGH is ceil(2560 / w); DX/DY origins are NTSC 636 / 50 (interlaced) / 25 and
// PAL 656 / 72 (interlaced) / 36. An unknown tv standard prints D_0026AE80 and leaves
// display untouched.
//

typedef struct {
    short inter;
    short omode;
    short ffmode;
} GParam;

typedef struct {
    unsigned long pmode;
    unsigned long smode2;
    unsigned long dispfb;
    unsigned long display;
    unsigned long bgcolor;
} DispEnv;

extern GParam *func_00100268(void);
extern int func_00122B58(const char *fmt, ...);

extern const char D_0026AE80[];

#define DISPLAY(dx, dy, magh, dw, dh)                                   \
    ((unsigned long)(dx) | ((unsigned long)(dh) << 44) |                \
     ((unsigned long)(magh) << 23) | ((unsigned long)(dy) << 12) |      \
     ((unsigned long)(dw) << 32))

void func_001002E0(DispEnv *disp, short psm, short w, short h, short dx, short dy)
{
    GParam *gp;

    gp = func_00100268();
    disp->pmode = 0x66;
    if (gp->inter) {
        if (gp->ffmode) {
            disp->smode2 = 3;
        } else {
            disp->smode2 = 1;
        }
    } else {
        disp->smode2 = 2;
    }
    disp->dispfb = ((unsigned long)(psm & 0xF) << 15) |
                   ((unsigned long)(((w + 63) >> 6) & 0x3F) << 9);
    if (gp->omode == 2) {
        if (gp->inter == 1) {
            int magh = (w + 2559) / w;
            disp->display = DISPLAY((636 + dx * magh) & 0xFFF, (dy + 50) & 0xFFF,
                                    magh - 1, magh * w - 1,
                                    (gp->ffmode ? h * 2 : h) - 1);
        } else {
            int magh = (w + 2559) / w;
            disp->display = DISPLAY((636 + dx * magh) & 0xFFF, (dy + 25) & 0xFFF,
                                    magh - 1, magh * w - 1, h - 1);
        }
    } else if (gp->omode == 3) {
        if (gp->inter == 1) {
            int magh = (w + 2559) / w;
            disp->display = DISPLAY((656 + dx * magh) & 0xFFF, (dy + 72) & 0xFFF,
                                    magh - 1, magh * w - 1,
                                    (gp->ffmode ? h * 2 : h) - 1);
        } else {
            int magh = (w + 2559) / w;
            disp->display = DISPLAY((656 + dx * magh) & 0xFFF, (dy + 36) & 0xFFF,
                                    magh - 1, magh * w - 1, h - 1);
        }
    } else {
        func_00122B58(D_0026AE80);
    }
    disp->bgcolor = 0;
}
