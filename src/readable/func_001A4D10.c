// NEARMISS func_001A4D10  (vram 0x001A4D10, 0x38C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 88.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scheduling and FPU colouring: with the segment inputs as // SPAD symbols, else-return shapes for
// the face tests and if/else stores of the normal sign, the control flow lines up; the original
// re-reads the scratchpad temporaries 0x70003680 / 0x70003684 right after storing them where mwcc
// 2.3.3 forwards the stored value (local, volatile and assignment-expression spellings measured),
// which shifts the FPU registers of the interpolation.
//
// The function links from the asm body in src/func_001A4D10.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003190 0x70003198 0x700031A0 0x700031A4 0x700031A8
extern float D_70003190;
extern float D_70003198;
extern float D_700031A0;
extern float D_700031A4;
extern float D_700031A8;
// Horizontal segment against one wall-face collision record (ACTOR_COLLISION.md
// record kind 0x2000; faces 1 / 2 are planes of constant x, faces 5 / 6 of
// constant z; faces 3 / 4 are func_001A4650's). The segment runs from
// (x0, z0) = (0x70003190, 0x70003198) to (x1, z1) = (0x700031A0, 0x700031A8)
// at height y = 0x700031A4, which must lie inside the face's y span. A face
// is only hit from its front: face 2 not while x decreases, face 1 not while
// it increases, likewise faces 6 / 5 for z. The crossing of the face plane
// (strictly between the segment ends) is interpolated through the scratchpad
// (0x70003680.. dx, dz, offset, crossing) and must fall strictly inside the
// face's other span. A hit publishes the point to 0x700031B0..B8, the angle
// halfword 0x700030CA = 0x2000, and the normal (+-1 on the face axis, 0 on
// the other two of 0x700030D4 / D8 / DC) and returns 1. Otherwise 0.
typedef struct WallFace {
    float x;    /* rec + 0x04 */
    float y;    /* rec + 0x08 */
    float z;    /* rec + 0x0C */
    float dx;   /* rec + 0x10 */
    float dy;   /* rec + 0x14 */
    float dz;   /* rec + 0x18 */
} WallFace;

int func_001A4D10(unsigned char *rec) {
    WallFace *f = (WallFace *)(rec + 4);
    unsigned char face = rec[2];
    float y0;
    float y1;
    float y;
    float x0;
    float x1;
    float z0;
    float z1;
    float xlo;
    float xhi;
    float zlo;
    float zhi;
    float lo;
    float hi;
    float c;
    float fx;
    float ddx;
    float ddz;
    float t;
    float fz;

    if ((unsigned int)(face - 3) < 2) {
        return 0;
    }
    if (f->dy < 0.0f) {
        y1 = f->y;
        y0 = f->y + f->dy;
    } else {
        y0 = f->y;
        y1 = f->y + f->dy;
    }
    y = D_700031A4;
    if (y < y0 || !(y <= y1)) {
        return 0;
    }
    x0 = D_70003190;
    x1 = D_700031A0;
    if (!(x0 <= x1)) {
        if (face == 2) {
            return 0;
        } else {
            xlo = x1;
            xhi = x0;
        }
    } else {
        if (face == 1) {
            return 0;
        } else {
            xlo = x0;
            xhi = x1;
        }
    }
    z0 = D_70003198;
    z1 = D_700031A8;
    if (!(z0 <= z1)) {
        if (face == 6) {
            return 0;
        } else {
            zlo = z1;
            zhi = z0;
        }
    } else {
        if (face == 5) {
            return 0;
        } else {
            zlo = z0;
            zhi = z1;
        }
    }
    if (face < 3) {
        fx = f->x;
        if (f->dz < 0.0f) {
            hi = f->z;
            lo = f->z + f->dz;
        } else {
            lo = f->z;
            hi = f->z + f->dz;
        }
        if (xlo < fx && !(xhi <= fx)) {
            *(float *)0x70003680 = x1 - x0;
            ddx = *(float *)0x70003680;
            *(float *)0x70003684 = z1 - z0;
            ddz = *(float *)0x70003684;
            *(float *)0x70003688 = fx - x0;
            c = z0 + ddz * *(float *)0x70003688 / ddx;
            *(float *)0x7000368C = c;
            if (!(c <= lo) && c < hi) {
                *(float *)0x700031B0 = fx;
                *(float *)0x700031B4 = y;
                *(float *)0x700031B8 = c;
                *(int *)0x700030DC = 0;
                *(int *)0x700030D8 = 0;
                *(short *)0x700030CA = 0x2000;
                if (face == 1) {
                    *(float *)0x700030D4 = 1.0f;
                } else {
                    *(float *)0x700030D4 = -1.0f;
                }
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    fz = f->z;
    if (f->dx < 0.0f) {
        hi = f->x;
        lo = f->x + f->dx;
    } else {
        lo = f->x;
        hi = f->x + f->dx;
    }
    if (zlo < fz && !(zhi <= fz)) {
        *(float *)0x70003680 = x1 - x0;
        ddx = *(float *)0x70003680;
        *(float *)0x70003684 = z1 - z0;
        ddz = *(float *)0x70003684;
        *(float *)0x70003688 = fz - z0;
        c = x0 + ddx * *(float *)0x70003688 / ddz;
        *(float *)0x7000368C = c;
        if (!(c <= lo) && c < hi) {
            *(float *)0x700031B0 = c;
            *(float *)0x700031B4 = y;
            *(float *)0x700031B8 = fz;
            *(int *)0x700030D8 = 0;
            *(int *)0x700030D4 = 0;
            *(short *)0x700030CA = 0x2000;
            if (face == 5) {
                *(float *)0x700030DC = 1.0f;
            } else {
                *(float *)0x700030DC = -1.0f;
            }
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
