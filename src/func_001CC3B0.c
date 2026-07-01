// NEARMISS func_001CC3B0  (vram 0x001CC3B0, 0x4E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// SAME wall as sibling func_001CC8A0 (already registered in docs/NEARMISS.md): CW 2.3.1.01 ABI/register-model mismatch. Target allocates ALL temporaries in caller-saved regs (t4-t9/a4-a7), needs only a -0x50 frame (s0-s3 saved); both available mwcc builds instead promote several locals into callee-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: GS/DMA "tall font" glyph-strip flush (docs/FINDINGS.md "Draw
// path" — func_001CC3B0 is the tall-font sprite flush, sibling of the small-
// font func_001CBC20). Draws the accumulated staging strip (built by
// func_001CC1E0's glyph loop into GS block 0x1B00) as a textured sprite. When
// arg7 (a per-instance color/outline struct) is non-null AND its byte+5 field
// is nonzero (an outline-thickness value, not just a boolean — it is ALSO
// added into the offset-corner math below), draws FIVE offset copies (center
// + up/down/left/right by 1 unit) in a two-tone outline/fill color — a classic
// drop-shadow/outlined-text render, each vertex an 8-field (0x60-byte) AD GS
// packet. Otherwise draws a plainer FIVE-vertex-pair (0x40-byte) sprite strip
// with a fixed color and an optional per-index alpha nudge from arg7[7]
// (clamped to 0xF, OR'd into bits 16-19 of the packed XY fields).
// arg0 = context slot index (into the per-frame render-context array
// *D_00275670), arg1/arg2 = screen x/y, arg3 = width, arg4 = z/depth (packed
// at bit 20 of the XYZ2 fields), arg5 = second-corner x/y span, arg6 = V
// coordinate (halved for the sub-pixel Y offset), arg7 = optional per-instance
// color/outline/alpha struct pointer.
extern char *D_00275670;
extern char D_002510C0[];
extern char D_00251140[];
extern char D_002511C0[];

void func_001CC3B0(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, unsigned char *arg7)
{
    char *slot;
    char *ra;
    unsigned int outline;
    unsigned int fill;
    unsigned int flag;
    int i;
    int dx, dy;
    unsigned int sel;
    long long z20;
    long long wz;
    long long half;

    slot = D_00275670 + arg0 * 4;
    *(char *)(*(int *)(slot + 0x10) + 3) = 0x30;
    *(void **)(*(int *)(slot + 0x10) + 4) = &D_002511C0;
    *(short *)(*(int *)(slot + 0x10) + 0) = 3;
    *(int *)(slot + 0x10) = *(int *)(slot + 0x10) + 0x10;

    if (arg7 != 0) {
        unsigned int hi = arg7[4] << 24;
        outline = hi | 0x100505;
        fill = hi | arg7[0];
        flag = arg7[5];
    } else {
        outline = 0x80100505;
        fill = 0x80808080;
        flag = 0;
    }

    if (flag != 0) {
        z20 = (long long)(int)(arg4 << 20);
        wz = (long long)(int)((arg3 << 4) | (arg4 << 20));
        half = (long long)(int)(arg6 >> 1);

        slot = D_00275670 + arg0 * 4;
        *(char *)(*(int *)(slot + 0x10) + 3) = 0x30;
        *(void **)(*(int *)(slot + 0x10) + 4) = &D_00251140;
        *(short *)(*(int *)(slot + 0x10) + 0) = 8;
        *(int *)(slot + 0x10) = *(int *)(slot + 0x10) + 0x10;

        slot = D_00275670 + arg0 * 4;
        *(char *)(*(int *)(slot + 0x10) + 3) = 0x10;
        *(int *)(*(int *)(slot + 0x10) + 4) = 0;
        *(short *)(*(int *)(slot + 0x10) + 0) = 0x1F;
        ra = *(char **)(slot + 0x10);
        *(int *)(slot + 0x10) = (int)(ra + 0x200);
        *(long long *)(ra + 0x10) = 0;
        ra = ra + 0x10 + 0x10;
        *(int *)(ra - 0x10 + 0xC) = 0x5000001E;

        for (i = 0; i < 5; i++) {
            switch (i) {
            case 0:
                dx = arg1;
                dy = arg2 + 1;
                sel = outline;
                break;
            case 1:
                dx = arg1 + 1;
                dy = arg2;
                sel = outline;
                break;
            case 2:
                dx = arg1 - 1;
                dy = arg2;
                sel = outline;
                break;
            case 3:
                dx = arg1;
                dy = arg2 - 1;
                sel = outline;
                break;
            default:
                dx = arg1;
                dy = arg2;
                sel = fill;
                break;
            }

            *(long long *)(ra + 0x0) = (long long)(int)(0x8001 | (sel << 20));
            *(long long *)(ra + 0x8) = (long long)(int)(0x34343431 | (4 << 20));
            *(long long *)(ra + 0x10) = z20;
            *(long long *)(ra + 0x18) = 0;
            *(long long *)(ra + 0x20) = (long long)(int)((dy << 20) | (dx << 4)) | half;
            *(long long *)(ra + 0x28) = wz;
            *(long long *)(ra + 0x30) = (long long)(int)(((dy + arg5) << 20) | ((dx + arg5) << 4)) | half;
            *(long long *)(ra + 0x38) = z20;
            *(long long *)(ra + 0x40) = (long long)(int)((dy << 20) | ((dx + arg5) << 4)) | half;
            *(long long *)(ra + 0x48) = wz;
            *(long long *)(ra + 0x50) = (long long)(int)(((dy + arg5) << 20) | (dx << 4)) | half;

            ra += 0x60;
        }
        return;
    }

    z20 = (long long)(int)(arg4 << 20);
    wz = (long long)(int)((arg3 << 4) | (arg4 << 20));
    half = (long long)(int)(arg6 >> 1);

    slot = D_00275670 + arg0 * 4;
    *(char *)(*(int *)(slot + 0x10) + 3) = 0x30;
    *(void **)(*(int *)(slot + 0x10) + 4) = &D_002510C0;
    *(short *)(*(int *)(slot + 0x10) + 0) = 8;
    *(int *)(slot + 0x10) = *(int *)(slot + 0x10) + 0x10;

    slot = D_00275670 + arg0 * 4;
    *(char *)(*(int *)(slot + 0x10) + 3) = 0x10;
    *(int *)(*(int *)(slot + 0x10) + 4) = 0;
    *(short *)(*(int *)(slot + 0x10) + 0) = 0x15;
    ra = *(char **)(slot + 0x10);
    *(int *)(slot + 0x10) = (int)(ra + 0x160);
    *(long long *)(ra + 0x10) = 0;
    ra = ra + 0x10 + 0x10;
    *(int *)(ra - 0x10 + 0xC) = 0x50000014;

    for (i = 0; i < 5; i++) {
        switch (i) {
        case 0:
            dx = arg1;
            dy = arg2 + 1;
            sel = outline;
            break;
        case 1:
            dx = arg1 + 1;
            dy = arg2;
            sel = outline;
            break;
        case 2:
            dx = arg1 - 1;
            dy = arg2;
            sel = outline;
            break;
        case 3:
            dx = arg1;
            dy = arg2 - 1;
            sel = outline;
            break;
        default:
            dx = arg1;
            dy = arg2;
            sel = fill;
            break;
        }

        *(long long *)(ra + 0x0) = (long long)(int)(0x8001 | (0x54000000 << 20));
        *(long long *)(ra + 0x8) = 0x43431;
        *(long long *)(ra + 0x10) = (long long)(int)sel;
        *(long long *)(ra + 0x18) = 0;
        *(long long *)(ra + 0x20) = (long long)(int)((dy << 20) | (dx << 4)) | half;
        *(long long *)(ra + 0x28) = wz;
        *(long long *)(ra + 0x30) = (long long)(int)(((dy + arg6) << 20) | ((dx + arg5) << 4)) | half;

        if (arg7 != 0) {
            unsigned int a = arg7[7];
            if ((int)a >= 0x10) {
                a = 0xF;
            }
            *(long long *)(ra + 0x20) |= (long long)(a << 16);
            *(long long *)(ra + 0x30) |= (long long)(a << 16);
        }

        ra += 0x40;
    }
}
