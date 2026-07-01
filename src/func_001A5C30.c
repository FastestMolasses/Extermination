// NEARMISS func_001A5C30  (vram 0x001A5C30, 0x808 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring / RA-order permutation (body-shape identical): (1) FP temp coloring swaps in the vertical-hit blocks and sloped-branch interpolation (target colors v into f1/hi into f2, mine f2/f1; ft-vs-fv shifts cascade); (2) int const materialization in the two vertical-hit store blocks: tar...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/laser segment-vs-actor clip test. obj+4 = float[5] {x, y, z, radius,
// half-height}. Globals 0x70003190/94/98 = segment start (x,y,z), 0x700031A0/A4/A8 =
// segment end. Projects the XZ segment direction against the actor center
// (func_001028E8 = 2D normalize/座 helper on seg/dir), finds the closest-approach
// point; if the XZ distance exceeds radius -> return 0. Otherwise computes the two
// XZ chord intersection points pts[0..2]/pts[3..5] (x,y,z pairs via sqrt
// func_0011E748), then classifies: if both dx and dz are below epsilon 1e-05
// (func_0011DF78 -> func_00128350 -> func_001000C0/func_00100110 magnitude-vs-double
// test), treats the segment as vertical and clips purely against the y slab
// [p1y-halfh, p1y+halfh], writing hit plane info to 0x700030CA (face id 0x4000 top /
// -0x8000 bottom), 0x700030D8 (normal y +-1.0f), 0x700031B0/B4/B8 (hit point) and
// returning 1. Otherwise interpolates y at the cylinder entry/exit (steep branch
// parameterizes by z, shallow by x), and if the crossing lies inside the actor's
// x/z extent and y slab, copies the entry point to D_700031B0.. (face 0x2000, side
// hit, normal from (hit-center)/radius into 0x700030D4/DC) or clips the y-interval
// ends against the slab (faces 0x4000/-0x8000 with interpolated x/z). Returns
// 1 = hit stored, 0 = miss.
//
// NEARMISS 96.33% (mwcc233; 991202 = 89.51% - no FPU-MAC fusion). Logic/structure
// fully recovered; residual is register-coloring permutation only (see wall notes).

extern void func_001028E8(float *a, float *b, float *c);
extern float func_0011E748(float a);
extern float func_0011DF78(float a);
extern int func_00128350(float a);
extern int func_001000C0(int a, double b);
extern int func_00100110(int a, double b);

extern float D_70003190;

int func_001A5C30(char *obj) {
    float pts[6];
    float dir[4];
    float seg[4];
    float *p = (float *)(obj + 4);
    float r2, r, rx, rz, t, d2;

    r2 = p[3] * p[3];
    {
        float xa, za, tx, tz;
        tx = *(float *)0x700031A0 - (xa = *(float *)0x70003190);
        dir[2] = tx;
        dir[0] = tx;
        tz = *(float *)0x700031A8 - (za = *(float *)0x70003198);
        dir[3] = tz;
        dir[1] = tz;
        rx = xa - p[0];
        rz = za - p[2];
    }
    seg[0] = dir[0];
    seg[1] = dir[1];
    seg[2] = rx;
    seg[3] = rz;
    func_001028E8(seg, seg, dir);
    t = -(seg[2] + seg[3]) / (seg[0] + seg[1]);
    rx += dir[0] * t;
    rz += dir[1] * t;
    d2 = rx * rx + rz * rz;
    if (r2 < d2) {
        return 0;
    }
    r = func_0011E748(r2 - d2);
    seg[2] = func_0011E748(dir[0] * dir[0] + dir[1] * dir[1]);
    seg[0] = dir[0] / seg[2];
    seg[1] = dir[1] / seg[2];
    pts[0] = rx + p[0] + seg[0] * -r;
    pts[2] = rz + p[2] + seg[1] * -r;
    pts[3] = rx + p[0] + seg[0] * r;
    pts[5] = rz + p[2] + seg[1] * r;
    dir[2] = dir[1];
    dir[1] = *(float *)0x700031A4 - *(float *)0x70003194;
    seg[0] = p[1] - p[4];
    seg[1] = p[1] + p[4];
    if (func_001000C0(func_00128350(func_0011DF78(dir[0])), 1e-05) != 0 &&
        func_001000C0(func_00128350(func_0011DF78(dir[2])), 1e-05) != 0) {
        if (func_00100110(func_00128350(func_0011DF78(dir[1])), 1e-05) != 0) {
            float lo = *(float *)0x70003194;
            float hi = *(float *)0x700031A4;
            if (!(lo <= hi)) {
                float v = seg[1];
                if (!(lo <= v) && hi < v) {
                    float hx = *(float *)0x70003190;
                    *(short *)0x700030CA = 0x4000;
                    *(int *)0x700030D8 = 0x3F800000;
                    *(float *)0x700031B0 = hx;
                    *(int *)0x700030DC = 0;
                    *(int *)0x700030D4 = 0;
                    *(float *)0x700031B8 = *(float *)0x70003198;
                    *(float *)0x700031B4 = v;
                    return 1;
                }
            } else {
                float v = seg[0];
                if (!(hi <= v) && lo < v) {
                    float hx = *(float *)0x70003190;
                    *(short *)0x700030CA = -0x8000;
                    *(int *)0x700030D8 = 0xBF800000;
                    *(float *)0x700031B0 = hx;
                    *(int *)0x700030DC = 0;
                    *(int *)0x700030D4 = 0;
                    *(float *)0x700031B8 = *(float *)0x70003198;
                    *(float *)0x700031B4 = v;
                    return 1;
                }
            }
        }
        return 0;
    }
    {
        float dz = dir[2];
        float dx = dir[0];
        if (!(dz <= dx)) {
            float *py = &pts[1];
            float za = *(float *)0x70003198;
            float k = dir[1];
            float ya = *(float *)0x70003194;
            *py = ya + (pts[2] - za) * k / dz;
            if (!(*py <= seg[0]) && *py < seg[1]) {
                float w = pts[2];
                float zb = *(float *)0x700031A8;
                if ((w < zb && !(w <= za)) || (!(w <= zb) && w < za)) {
                    float *src = pts;
                    float *dst = &D_70003190;
                    int i = 0;
                    float x0;
                    do {
                        dst[8] = *src;
                        i++;
                        src++;
                        dst++;
                    } while (i < 3);
                    x0 = pts[0];
                    *(short *)0x700030CA = 0x2000;
                    *(float *)0x700030D4 = (x0 - p[0]) / p[3];
                    *(int *)0x700030D8 = 0;
                    *(float *)0x700030DC = (pts[2] - p[2]) / p[3];
                    return 1;
                }
            }
            pts[4] = ya + (pts[5] - za) * k / dz;
        } else {
            float *py = &pts[1];
            float k = dir[1];
            float xa = *(float *)0x70003190;
            float ya = *(float *)0x70003194;
            *py = ya + (pts[0] - xa) * k / dx;
            {
                float w = pts[0];
                float xb = *(float *)0x700031A0;
                if ((w < xb && !(w <= xa)) || (!(w <= xb) && w < xa)) {
                    float y = *py;
                    if (!(y <= seg[0]) && y < seg[1]) {
                        float *src = pts;
                        float *dst = &D_70003190;
                        int i = 0;
                        float x0;
                        do {
                            dst[8] = *src;
                            i++;
                            src++;
                            dst++;
                        } while (i < 3);
                        x0 = pts[0];
                        *(short *)0x700030CA = 0x2000;
                        *(float *)0x700030D4 = (x0 - p[0]) / p[3];
                        *(int *)0x700030D8 = 0;
                        *(float *)0x700030DC = (pts[2] - p[2]) / p[3];
                        return 1;
                    }
                }
            }
            pts[4] = ya + (pts[3] - xa) * k / dx;
        }
    }
    {
        float y1 = pts[1];
        float y2 = pts[4];
        if (!(y1 <= y2)) {
            float b = seg[1];
            if (!(y1 <= b) && y2 < b) {
                float t2 = (b - y1) / (y2 - y1);
                float oz;
                *(float *)0x700031B0 = pts[0] + t2 * (pts[3] - pts[0]);
                oz = pts[2] + t2 * (pts[5] - pts[2]);
                *(short *)0x700030CA = 0x4000;
                *(int *)0x700030D8 = 0x3F800000;
                *(int *)0x700030DC = 0;
                *(int *)0x700030D4 = 0;
                *(float *)0x700031B8 = oz;
                *(float *)0x700031B4 = b;
                return 1;
            }
        } else {
            float b = seg[0];
            if (y1 < b && !(y2 <= b)) {
                float t2 = (b - y1) / (y2 - y1);
                float oz;
                *(float *)0x700031B0 = pts[0] + t2 * (pts[3] - pts[0]);
                oz = pts[2] + t2 * (pts[5] - pts[2]);
                *(short *)0x700030CA = -0x8000;
                *(int *)0x700030D8 = 0xBF800000;
                *(int *)0x700030DC = 0;
                *(int *)0x700030D4 = 0;
                *(float *)0x700031B8 = oz;
                *(float *)0x700031B4 = b;
                return 1;
            }
        }
    }
    return 0;
}
