// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 49.26% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - written in the SDK libgraph shape: the draw environment is a struct of
//    64-bit GS registers, SCISSOR is a bit-field struct of unsigned long
//    fields, and the offset is one SCE_GS_SET_XYOFFSET-style macro expression.
//    The 11-bit SCAX1/SCAY1 fields promote to int (the 32-to-64 sign
//    extension), "(u_long)(f + 1) >> 1" is the 64-bit logical halving, the
//    short centre is widened to long before the subtraction, and the half
//    pixel is a "(halfoff ? 8 : 0)" term inside the OFY argument (that is what
//    splits the OFY shift into two paths).

// SDK libgraph: sets a draw environment's XYOFFSET so that the screen centre
// (centerx, centery) maps to the middle of its scissor area: OFX = (centerx -
// (SCAX1 + 1) / 2) * 16 and OFY = (centery - (SCAY1 + 1) / 2) * 16, plus a half
// pixel (8) on y when halfoff is set (the odd field). func_00101810 and
// func_001015A8 are two identical copies.
typedef unsigned long u_long;
typedef struct {
    u_long SCAX0:11; u_long pad11:5; u_long SCAX1:11; u_long pad27:5;
    u_long SCAY0:11; u_long pad43:5; u_long SCAY1:11; u_long pad59:5;
} sceGsScissor;
typedef struct {
    u_long OFX:16; u_long pad16:16; u_long OFY:16; u_long pad48:16;
} sceGsXyoffset;
typedef struct {
    u_long frame1; long frame1addr;
    u_long zbuf1; long zbuf1addr;
    sceGsXyoffset xyoffset1; long xyoffset1addr;
    sceGsScissor scissor1; long scissor1addr;
} sceGsDrawEnv1;
#define SCE_GS_SET_XYOFFSET(ofx, ofy) ((u_long)(ofx) | ((u_long)(ofy) << 32))

void func_00101810(sceGsDrawEnv1 *draw, short centerx, short centery, short halfoff)
{
    *(u_long *)&draw->xyoffset1 = SCE_GS_SET_XYOFFSET(
        ((long)centerx - ((u_long)(draw->scissor1.SCAX1 + 1) >> 1)) << 4,
        (((long)centery - ((u_long)(draw->scissor1.SCAY1 + 1) >> 1)) << 4) + (halfoff ? 8 : 0));
}
