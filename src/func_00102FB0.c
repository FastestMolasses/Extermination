// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Fix read from the original instructions: the normal's components are read
// into locals declared inside each branch (with function-scope locals the
// register allocation differs), and the cells are assigned column by column.
//
// Builds a 4x4 matrix (column-major, m[column][row]) from the vector n and the
// point (x, y, z), with d = x*n0 + y*n1 + z*n2:
//  - mode != 0: m[c][r] = p[c] * n[r] plus k = 1 - d on the diagonal, the
//    fourth row (m[c][3]) is p, the fourth column is -n with m[3][3] = k - 1;
//  - mode == 0: every cell of the same outer product (diagonal minus d,
//    fourth row 0, fourth column -n, m[3][3] = -d) is scaled by -1 / d.
// (A projection onto the plane with normal n; exact geometric use unconfirmed.)

void func_00102FB0(float m[4][4], float *n, int mode, float x, float y, float z)
{
    if (mode) {
        float n0, n1, n2, d;

        n0 = n[0];
        n1 = n[1];
        n2 = n[2];
        d = 1.0f - (x * n0 + y * n1 + z * n2);

        m[0][0] = x * n0 + d;
        m[1][0] = y * n0;
        m[2][0] = z * n0;
        m[3][0] = -n0;
        m[0][1] = x * n1;
        m[1][1] = y * n1 + d;
        m[2][1] = z * n1;
        m[3][1] = -n1;
        m[0][2] = x * n2;
        m[1][2] = y * n2;
        m[2][2] = z * n2 + d;
        m[3][2] = -n2;
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
        m[3][3] = d - 1.0f;
    } else {
        float n0, n1, n2, d, r;

        n0 = n[0];
        n1 = n[1];
        n2 = n[2];
        d = x * n0 + y * n1 + z * n2;
        r = -1.0f / d;

        m[0][0] = r * (x * n0 - d);
        m[1][0] = r * (y * n0);
        m[2][0] = r * (z * n0);
        m[3][0] = r * -n0;
        m[0][1] = r * (x * n1);
        m[1][1] = r * (y * n1 - d);
        m[2][1] = r * (z * n1);
        m[3][1] = r * -n1;
        m[0][2] = r * (x * n2);
        m[1][2] = r * (y * n2);
        m[2][2] = r * (z * n2 - d);
        m[3][2] = r * -n2;
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        m[3][3] = r * -d;
    }
}
