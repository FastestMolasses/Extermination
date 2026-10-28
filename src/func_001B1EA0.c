// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 92.91% NEARMISS).
// Fixes read from the original instructions:
//  - Vertices are indexed (v[i * 4 + c], v[j * 4 + c]), not walked with a
//    separate pointer; the compiler's own strength-reduced pointer then lands
//    in the saved register right after the counter, below the parameters.
//  - The next-vertex index is an if/else, not a ternary.
//  - Each edge computes the far vertex's two components first, then the near
//    vertex's (bx, bz, ax, az), with the temporaries declared in that order;
//    that fixed the FPU colouring of all three loops.

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
    float bx;
    float bz;
    float ax;
    float az;
    int i;
    int j;

    if (n < 3) {
        return 0;
    }
    switch (plane) {
    case 0:
        for (i = 0; i < n; i++) {
            if (i == n - 1) {
                j = 0;
            } else {
                j = i + 1;
            }
            bx = v[j * 4 + 0] - p[0];
            bz = v[j * 4 + 2] - p[2];
            ax = v[i * 4 + 0] - p[0];
            az = v[i * 4 + 2] - p[2];
            sum += func_0011E620(bz * ax - bx * az, bx * ax + bz * az);
        }
        break;
    case 1:
        for (i = 0; i < n; i++) {
            if (i == n - 1) {
                j = 0;
            } else {
                j = i + 1;
            }
            bx = v[j * 4 + 0] - p[0];
            bz = v[j * 4 + 1] - p[1];
            ax = v[i * 4 + 0] - p[0];
            az = v[i * 4 + 1] - p[1];
            sum += func_0011E620(bz * ax - bx * az, bx * ax + bz * az);
        }
        break;
    case 2:
        for (i = 0; i < n; i++) {
            if (i == n - 1) {
                j = 0;
            } else {
                j = i + 1;
            }
            bx = v[j * 4 + 1] - p[1];
            bz = v[j * 4 + 2] - p[2];
            ax = v[i * 4 + 1] - p[1];
            az = v[i * 4 + 2] - p[2];
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
