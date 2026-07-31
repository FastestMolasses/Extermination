// NEARMISS func_001002E0  (vram 0x001002E0, 0x270 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 65.22% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler + regalloc wall inside the four DISPLAY-building arms. Head is byte-identical: prologue with the five short sign-extends, func_00100268 call, disp[0]=0x66, the lh/lhu pair on mode[0], the 2/3/1 interlace-code selection and its single sd, and the whole DISPFB expression (andi/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: build a GS display configuration block (Sony SDK "lowmem" region).
//
// func_00100268() returns the current video-mode descriptor:
//   mode[0] = field/interlace mode, mode[1] = tv standard (2 = NTSC, 3 = PAL),
//   mode[2] = frame/field (FFMD).
//
// The five 64-bit words written to `disp` are:
//   disp[0] = 0x66            (block id)
//   disp[1] = interlace code  (2 = non-interlaced, 3 = FFMD frame, 1 = field)
//   disp[2] = DISPFB value    (FBW bits 9..14, PSM bits 15..19)
//   disp[3] = DISPLAY value   (DX 0..11, DY 12..22, MAGH 23..26,
//                              DW 32..43, DH 44..54)
//   disp[4] = 0
//
// DX/DY origins are the standard NTSC (0x27C / 0x32 / 0x19) and
// PAL (0x290 / 0x48 / 0x24) display offsets.
//

extern short *func_00100268(void);
extern void func_00122B58();

extern const char D_0026AE80[];

void func_001002E0(long *disp, short psm, short width, short height, short x,
                   short y) {
    short *mode;
    short m0;
    unsigned short field;
    int magh;
    int mag;
    long dx;
    int dy;
    int dw;
    int dh;

    mode = func_00100268();
    disp[0] = 0x66;

    m0 = mode[0];
    field = *(unsigned short *)mode;
    if (m0 != 0) {
        if (mode[2] != 0) {
            disp[1] = 3;
        } else {
            disp[1] = 1;
        }
    } else {
        disp[1] = 2;
    }

    disp[2] = ((long)(psm & 0xF) << 15) | ((long)(((width + 63) >> 6) & 0x3F) << 9);

    if (mode[1] == 2) {
        if ((short)field == 1) {
            magh = (width + 0x9FF) / width;
            dy = (y + 0x32) & 0xFFF;
            dx = ((long)(x * magh) + 0x27C) & 0xFFF;
            dw = magh * width - 1;
            mag = magh - 1;
            if (mode[2] != 0) {
                dh = height * 2 - 1;
            } else {
                dh = height - 1;
            }
            disp[3] = dx | ((long)dy << 12) | ((long)mag << 23) |
                      ((long)dw << 32) | ((long)dh << 44);
        } else {
            magh = (width + 0x9FF) / width;
            dy = (y + 0x19) & 0xFFF;
            dx = ((long)(x * magh) + 0x27C) & 0xFFF;
            dw = magh * width - 1;
            mag = magh - 1;
            dh = height - 1;
            disp[3] = dx | ((long)dy << 12) | ((long)mag << 23) |
                      ((long)dw << 32) | ((long)dh << 44);
        }
    } else if (mode[1] == 3) {
        if ((short)field == 1) {
            magh = (width + 0x9FF) / width;
            dy = (y + 0x48) & 0xFFF;
            dx = ((long)(x * magh) + 0x290) & 0xFFF;
            dw = magh * width - 1;
            mag = magh - 1;
            if (mode[2] != 0) {
                dh = height * 2 - 1;
            } else {
                dh = height - 1;
            }
            disp[3] = dx | ((long)dy << 12) | ((long)mag << 23) |
                      ((long)dw << 32) | ((long)dh << 44);
        } else {
            magh = (width + 0x9FF) / width;
            dy = (y + 0x24) & 0xFFF;
            dx = ((long)(x * magh) + 0x290) & 0xFFF;
            dw = magh * width - 1;
            mag = magh - 1;
            dh = height - 1;
            disp[3] = dx | ((long)dy << 12) | ((long)mag << 23) |
                      ((long)dw << 32) | ((long)dh << 44);
        }
    } else {
        func_00122B58(D_0026AE80);
    }

    disp[4] = 0;
}
