// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Circle/range collision-resolve between two actors a0,a1. Computes the
// horizontal (X/Z) separation dx,dy between a0+0xA0/0xA8 and a1+0xB0/0xB8,
// takes its length via func_0011E748 (sqrtf of dx*dx+dy*dy, FMAC), and if the
// distance is within the summed radii (each actor's radius is the first float
// of its +0x30 sub-struct) checks the vertical/secondary axis (the half-radius
// box test at +0xA4/+0xB4) and that a1 is not flagged-busy (a1[3]==0). On a hit
// it pushes a1 out along the contact angle: ang = normalize(atan2(dx,dy)) via
// func_0011E620/func_001B1470, then a1.x/a1.z = a0.x/a0.z - sum*cos/sin(ang)
// (func_0011E2A8=cos, func_0011DE90=sin), and sets a1[0xB]=1 unless a0 is
// flagged (a0[0]&4). The radii p0/p1 must be reloaded AFTER the sqrtf call
// (not hoisted) so only a0/a1 occupy callee-saved regs (frame 0x40); hoisting
// them grows the frame to 0x60 and breaks the match.
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 stalls at
// 96.59% (clean-store delay-slot codegen), 2.3.3 reaches objdiff 100%.
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern float func_0011E620(float, float);
extern float func_0011E748(float);
extern float func_001B1470(float);

void func_001A9E00(unsigned char *a0, unsigned char *a1) {
    float dx = *(float *)(a0 + 0xA0) - *(float *)(a1 + 0xB0);
    float dy = *(float *)(a0 + 0xA8) - *(float *)(a1 + 0xB8);
    float dist = func_0011E748(dx * dx + dy * dy);
    unsigned char *p0 = *(unsigned char **)(a0 + 0x30);
    unsigned char *p1 = *(unsigned char **)(a1 + 0x30);
    float sum = *(float *)p0 + *(float *)p1;
    if (dist <= sum) {
        float h = *(float *)(p0 + 4) / 2.0f;
        float d = (*(float *)(a0 + 0xA4) + h) - *(float *)(a1 + 0xB4);
        if (d < 0.0f) {
            d = -d;
        }
        if (d <= (h + *(float *)(p1 + 4) / 2.0f) && *(unsigned char *)(a1 + 3) == 0) {
            float ang = func_001B1470(func_0011E620(dx, dy));
            float s = sum * func_0011DE90(ang);
            *(float *)(a1 + 0xB0) = *(float *)(a0 + 0xA0) - sum * func_0011E2A8(ang);
            *(float *)(a1 + 0xB8) = *(float *)(a0 + 0xA8) - s;
            if (!(*(unsigned char *)(a0 + 0) & 4)) {
                *(unsigned char *)(a1 + 0xB) = 1;
            }
        }
    }
}
