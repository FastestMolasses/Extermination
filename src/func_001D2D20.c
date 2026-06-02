extern void func_001029C0(float *m);  // load 4x4 identity

// Builds a perspective projection matrix in `m` (4x4, row-major).
//   m[0][0] = focal / (width * 0.5)
//   m[1][1] = focal / (height * 0.5)
//   m[2][2] = (far + near) / (far - near)
//   m[2][3] = 1.0
//   m[3][2] = (-2 * far * near) / (far - near)
//   m[3][3] = 0
// (a Z-divide-by-W projection: row 2 carries depth, row 3 the W term).
void func_001D2D20(float *m, float focal, float width, float height,
                   float near, float far) {
    func_001029C0(m);
    m[0]  = focal / (0.5f * width);
    m[5]  = focal / (0.5f * height);
    m[10] = (far + near) / (far - near);
    m[14] = (-2.0f * (far * near)) / (far - near);
    m[11] = 1.0f;
    m[15] = 0.0f;
}
