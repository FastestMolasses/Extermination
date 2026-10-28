// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK VU0-library math: builds the view-to-screen matrix. m becomes the
// perspective matrix (scrz on the x/y diagonal, the [2][3] and [3][2] cells 1,
// [2][2] and [3][3] 0); mt is the screen mapping (scale ax/ay/az on the
// diagonal, translation cx/cy/cz in row 3), where az and cz map the depth
// range nearz..farz onto zmin..zmax:
//   cz = (zmin * farz - zmax * nearz) / (farz - nearz)
//   az = farz * nearz * (zmax - zmin) / (farz - nearz)
// The result is m = mt applied after m (func_001026D0(m, mt, m)).
// func_001029C0 sets a matrix to identity. farz is the ninth float argument
// and arrives on the stack.

extern void func_001029C0(float (*m)[4]);
extern void func_001026D0(float (*dst)[4], float (*a)[4], float (*b)[4]);

void func_00102EA8(float m[4][4], float scrz, float ax, float ay, float cx,
                   float cy, float zmin, float zmax, float nearz, float farz)
{
    float az, cz;
    float mt[4][4];

    cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
    az = farz * nearz * (-zmin + zmax) / (-nearz + farz);

    func_001029C0(m);
    m[0][0] = scrz;
    m[1][1] = scrz;
    m[2][2] = 0.0f;
    m[3][3] = 0.0f;
    m[3][2] = 1.0f;
    m[2][3] = 1.0f;

    func_001029C0(mt);
    mt[0][0] = ax;
    mt[1][1] = ay;
    mt[2][2] = az;
    mt[3][0] = cx;
    mt[3][1] = cy;
    mt[3][2] = cz;

    func_001026D0(m, mt, m);
}
