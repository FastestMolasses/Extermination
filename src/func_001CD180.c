// NEARMISS func_001CD180  (vram 0x001CD180, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 46.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern int   D_00275C04;        /* integer view depth of the last projection */
extern IVec4 D_70003600;        /* 12.4 screen position of that projection   */
extern Vec4  D_70003610;        /* scratchpad working quad (aliased as int)  */
extern Vec4  D_70003A40[4];     /* screen-scale projection matrix, scratchpad */

/* lqc2 the matrix, zero the z column's x/y (vsub.xy), transform, divide by w
   and vftoi4.xy the x/y lanes into 12.4. COP2 macro mode, no C spelling. */
extern void vu0_extent_project_xy(Vec4 *quad, const Vec4 *m);

int func_001CD180(float width, float height)
{
    int cx, cy;
    int hw, hh;

    D_70003610.x = 0.5f * width;
    D_70003610.y = 0.5f * height;
    D_70003610.z = (float)D_00275C04;
    /* .w is left stale and never read */

    vu0_extent_project_xy(&D_70003610, D_70003A40);

    cx = D_70003600.x;                      /* 12.4 screen centre */
    cy = D_70003600.y;
    hw = *(int *)&D_70003610.x;             /* 12.4 half extents  */
    hh = *(int *)&D_70003610.y;

    if (cx - hw >= 0x9001) return 0;
    if (cy - hh >= 0x8701) return 0;
    if (cx + hw <  0x7000) return 0;
    if (cy + hh <  0x7900) return 0;

    if (hw < hh) {
        *(int *)&D_70003610.x = hh;         /* keep the larger extent */
        hw = hh;
    }
    return (hw >> 3) + 1;
}
