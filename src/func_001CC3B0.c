// NEARMISS func_001CC3B0  (vram 0x001CC3B0, 0x4E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.13% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). Body rewritten
// from the original instructions (was 62.16%): same instruction count and order apart from
// register choice. Remaining differences: register colouring only (the original keeps the
// first header block's temporaries and the outline/fill setup in s0-s2 and the loop pointer
// in ra; ours picks t-registers), plus the scheduling that follows from it. Survives
// declaration-order sweeps, block-scoped loop locals and expression spellings; a 300 s
// 2.3.3 permuter run found nothing valid.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: GS/DMA "tall font" glyph-strip flush (docs/FINDINGS.md "Draw path";
// sibling of the small-font func_001CBC20). First appends a DMA ref tag (byte +3 =
// 0x30, qwc 3) pointing at D_002511C0 to the per-frame render-context slot
// &(*D_00275670)[arg0] (cursor slot[4]). arg7 is an optional style record: word +0
// fill RGBA, byte +4 alpha (shifted into bits 24-31 of both colours), byte +5
// outline thickness, byte +7 alpha nudge. Without arg7 the colours are 0x80100505
// (outline) and 0x80808080 (fill) and there is no outline.
// With an outline thickness: a ref tag to D_00251140 (qwc 8), then a 0x1F-qword
// packet (GIFtag 0x5000001E) of five 0x60-byte sprites: the strip drawn at
// (x, y-1), (x+1, y), (x, y+1), (x-1, y) in the outline colour and at (x, y) in the
// fill colour; the top edge is shifted right by the thickness (italic/slant), the
// bottom edge sits at y + arg6/2, width arg5; arg3/arg4 are packed as the U/V
// texture words. Otherwise: a ref tag to D_002510C0 (qwc 8), then a 0x15-qword
// packet (GIFtag 0x50000014) of five 0x40-byte sprites with the same offsets and
// colours; with arg7 its nudge byte (clamped to 0xF) is ORed into bits 16-19 of
// both XYZ words. arg0 = context slot index, arg1/arg2 = screen x/y.
typedef struct {
    unsigned int rgba;
    unsigned char alpha;
    unsigned char outline;
    unsigned char pad6;
    unsigned char nudge;
} FontStyle;

extern char **D_00275670;
extern char D_002510C0[];
extern char D_00251140[];
extern char D_002511C0[];

void func_001CC3B0(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, FontStyle *arg7)
{
    char **slot;
    char *p;
    unsigned long long *q;
    unsigned int hi;
    int outline;
    int fill;
    int flag;
    int i;
    int dx;
    int dy;
    int col;
    int half;
    int a;

    slot = &D_00275670[arg0];
    *(unsigned char *)(slot[4] + 3) = 0x30;
    *(char **)(slot[4] + 4) = D_002511C0;
    *(short *)(slot[4] + 0) = 3;
    slot[4] = slot[4] + 0x10;

    if (arg7 != 0) {
        hi = arg7->alpha << 24;
        outline = hi | 0x100505;
        fill = hi | arg7->rgba;
        flag = arg7->outline;
    } else {
        outline = 0x80100505;
        fill = 0x80808080;
        flag = 0;
    }

    if (flag != 0) {

        slot = &D_00275670[arg0];
        *(unsigned char *)(slot[4] + 3) = 0x30;
        *(char **)(slot[4] + 4) = D_00251140;
        *(short *)(slot[4] + 0) = 8;
        slot[4] = slot[4] + 0x10;

        slot = &D_00275670[arg0];
        *(unsigned char *)(slot[4] + 3) = 0x10;
        *(int *)(slot[4] + 4) = 0;
        *(short *)(slot[4] + 0) = 0x1F;
        p = slot[4];
        q = (unsigned long long *)(p + 0x10);
        slot[4] = p + 0x200;
        *(unsigned __int128 *)(p + 0x10) = 0;
        q = (unsigned long long *)((char *)q + 0x10);
        *(int *)(p + 0x1C) = 0x5000001E;

        for (i = 0; i < 5; i++) {
            switch (i) {
            case 0:
                dy = arg2 - 1;
                dx = arg1;
                col = outline;
                break;
            case 1:
                dx = arg1 + 1;
                dy = arg2;
                col = outline;
                break;
            case 2:
                dy = arg2 + 1;
                dx = arg1;
                col = outline;
                break;
            case 3:
                dx = arg1 - 1;
                dy = arg2;
                col = outline;
                break;
            case 4:
                dx = arg1;
                dy = arg2;
                col = fill;
                break;
            }
            q[0] = (unsigned long long)0x8001 | ((unsigned long long)0x94000000 << 32);
            q[1] = (unsigned long long)0x34343431 | ((unsigned long long)4 << 32);
            q[2] = (long long)col;
            q[3] = 0;
            q[4] = (long long)(int)(((dx + (unsigned char)flag) << 4) | (dy << 20)) | ((unsigned long long)0xFFFFFF << 32);
            q[5] = (long long)(arg4 << 20);
            q[6] = (long long)(int)((dx << 4) | ((dy + (arg6 >> 1)) << 20)) | ((unsigned long long)0xFFFFFF << 32);
            q[7] = (long long)(arg3 << 4);
            q[8] = (long long)(int)(((dx + (unsigned char)flag + arg5) << 4) | (dy << 20)) | ((unsigned long long)0xFFFFFF << 32);
            q[9] = (long long)((arg3 << 4) | (arg4 << 20));
            q[10] = (long long)(int)(((dx + arg5) << 4) | ((dy + (arg6 >> 1)) << 20)) | ((unsigned long long)0xFFFFFF << 32);
            q += 12;
        }
        return;
    }

    {
        long long uv = (long long)((arg3 << 4) | (arg4 << 20));
        half = arg6 >> 1;

        slot = &D_00275670[arg0];
        *(unsigned char *)(slot[4] + 3) = 0x30;
        *(char **)(slot[4] + 4) = D_002510C0;
        *(short *)(slot[4] + 0) = 8;
        slot[4] = slot[4] + 0x10;

        slot = &D_00275670[arg0];
        *(unsigned char *)(slot[4] + 3) = 0x10;
        *(int *)(slot[4] + 4) = 0;
        *(short *)(slot[4] + 0) = 0x15;
        p = slot[4];
        q = (unsigned long long *)(p + 0x10);
        slot[4] = p + 0x160;
        *(unsigned __int128 *)(p + 0x10) = 0;
        q = (unsigned long long *)((char *)q + 0x10);
        *(int *)(p + 0x1C) = 0x50000014;

        for (i = 0; i < 5; i++) {
            switch (i) {
            case 0:
                dy = arg2 - 1;
                dx = arg1;
                col = outline;
                break;
            case 1:
                dx = arg1 + 1;
                dy = arg2;
                col = outline;
                break;
            case 2:
                dy = arg2 + 1;
                dx = arg1;
                col = outline;
                break;
            case 3:
                dx = arg1 - 1;
                dy = arg2;
                col = outline;
                break;
            case 4:
                dx = arg1;
                dy = arg2;
                col = fill;
                break;
            }
            q[0] = (unsigned long long)0x8001 | ((unsigned long long)0x54000000 << 32);
            q[1] = 0x43431;
            q[2] = (long long)col;
            q[3] = 0;
            q[4] = (long long)(int)((dx << 4) | (dy << 20)) | ((unsigned long long)0xFFFFFF << 32);
            q[5] = uv;
            q[6] = (long long)(int)(((dx + arg5) << 4) | ((dy + half) << 20)) | ((unsigned long long)0xFFFFFF << 32);
            if (arg7 != 0) {
                a = arg7->nudge;
                if (a >= 0x10) {
                    a = 0xF;
                }
                q[4] |= (long long)(a << 16);
                q[6] |= (long long)(a << 16);
            }
            q += 8;
        }
    }
}
