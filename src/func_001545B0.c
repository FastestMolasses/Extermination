// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS (resolved s33): tendril-point validity test -- is world
// point (x X, z Z) inside the ELLIPSE of semi-axes 0.92*recX (X) /
// 0.92*recZ (Z) centered on the actor origin +0xB0? rec = D_00248120 +
// self[0xD]*20. Heading origin->point via atan2 func_001B1240, ellipse
// radius-at-angle r^2 = (a^2 b^2)/(a^2 sin^2 + b^2 cos^2) (a = 0.92*recZ,
// b = 0.92*recX; sin = func_0011E2A8, cos = func_0011DE90) vs distXZ^2.
// (locals c/s keep pre-s45 names: c holds sin(ang), s holds cos(ang).)
// mwcc 2.3.3 reproduces the mula.s/nop/madd.s ACC-hazard nop that 991202
// could not (991202 stalls at 95.45%). The `goto inside` tail reproduces
// CW's branch-on-true + duplicated dead `li v0,1`.
extern unsigned char D_00248120[];
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_0011E2A8(float a); /* sin */
extern float func_0011DE90(float a); /* cos */

int func_001545B0(unsigned char *self, float x, float z) {
    unsigned char *rec;
    float ang, a, b, c, s, dx, dz, r2, d2;
    rec = D_00248120 + self[0xD] * 20;
    ang = func_001B1240(self + 0xB0, x, z);
    a = 0.9200000166893005f * *(float *)(rec + 8);
    b = 0.9200000166893005f * *(float *)(rec + 0);
    a = a * a;
    b = b * b;
    c = func_0011E2A8(ang);
    c = c * c;
    s = func_0011DE90(ang);
    s = s * s;
    dx = x - *(float *)(self + 0xB0);
    dz = z - *(float *)(self + 0xB8);
    r2 = (a * b) / (a * c + b * s);
    d2 = dx * dx + dz * dz;
    if (d2 <= r2) goto inside;
    return 0;
inside:
    return 1;
}
