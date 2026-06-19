// COMPILER: eegcc
// CFLAGS: -O2
typedef struct { float x, y, z, w; } Vec;
typedef struct { Vec col0, col1, col2, col3; } Mat;

extern void func_00102900(Vec *dst, Vec *src, float s);
extern void func_00102760(Vec *dst, Vec *src);
extern void func_00102798(Mat *dst, Mat *src);

void func_00102D80(Mat *m, Vec *vx, Vec *vy, Vec *vz) {
    Vec tmp;
    func_00102900(&tmp, vx, -1.0f);
    func_00102760(&m->col0, &tmp);
    func_00102900(&tmp, vy, -1.0f);
    func_00102760(&m->col1, &tmp);
    func_00102900(&tmp, vz, -1.0f);
    func_00102760(&m->col2, &tmp);
    {
        float zero = 0.0f;
        m->col3.w = 1.0f;
        m->col3.z = zero;
        m->col3.y = zero;
        m->col3.x = zero;
    }
    func_00102798(m, m);
}
