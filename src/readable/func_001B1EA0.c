// NEARMISS func_001B1EA0  (vram 0x001B1EA0, 0x294 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 92.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Saved-register allocation (the original numbers the loop counters below the parameters) and FPU
// colouring of the edge vectors; declaration order and parameter copies measured.
//
// The function links from the asm body in src/func_001B1EA0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Point-in-polygon test by winding angle. v is an array of n vec4 vertices
// (16 bytes each) and p the query point; the polygon is projected on a
// coordinate plane chosen by plane: 0 = (x, z), 1 = (x, y), 2 = (y, z).
// For every edge (v[i], v[i + 1], wrapping to v[0]) the signed angle
// between the two vertices as seen from p, atan2f(cross, dot)
// (func_0011E620), is summed. Plane 2 measures the angle the other way round.
// Returns 1 when |sum| exceeds pi (p inside), 0 otherwise or for fewer than
// three vertices.
extern float func_0011E620(float y, float x);

int func_001B1EA0(int plane, float *p, float *v, int n) {
    float sum = 0.0f;
    float *a;
    float *b;
    float ax;
    float az;
    float bx;
    float bz;
    int i;
    int j;

    if (n < 3) {
        return 0;
    }
    switch (plane) {
    case 0:
        for (i = 0, a = v; i < n; i++, a += 4) {
            j = (i == n - 1) ? 0 : i + 1;
            b = v + j * 4;
            bx = b[0] - p[0];
            ax = a[0] - p[0];
            bz = b[2] - p[2];
            az = a[2] - p[2];
            sum += func_0011E620(bz * ax - bx * az, bx * ax + bz * az);
        }
        break;
    case 1:
        for (i = 0, a = v; i < n; i++, a += 4) {
            j = (i == n - 1) ? 0 : i + 1;
            b = v + j * 4;
            bx = b[0] - p[0];
            ax = a[0] - p[0];
            bz = b[1] - p[1];
            az = a[1] - p[1];
            sum += func_0011E620(bz * ax - bx * az, bx * ax + bz * az);
        }
        break;
    case 2:
        for (i = 0, a = v; i < n; i++, a += 4) {
            j = (i == n - 1) ? 0 : i + 1;
            b = v + j * 4;
            bx = b[1] - p[1];
            ax = a[1] - p[1];
            az = a[2] - p[2];
            bz = b[2] - p[2];
            sum += func_0011E620(bx * az - bz * ax, bx * ax + bz * az);
        }
        break;
    }
    if (sum < 0.0f) {
        if (sum < -3.1415927f) {
            return 1;
        } else {
            return 0;
        }
    } else if (sum > 3.1415927f) {
        return 1;
    } else {
        return 0;
    }
}
