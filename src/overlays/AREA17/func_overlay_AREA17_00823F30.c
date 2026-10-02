// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00823F70 (splat/link name 00823F30; overlay code
//  is linked 0x40 below where it runs), 0x310 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: script callback (script 0x8270A0, op09 record 0x827220), called with
//  (self, record, work). Record +4 0: 0x969A80..0x969A88 = the midpoint of
//  D_00810350 and the point 0x827520, 0x969A8C = their horizontal distance
//  (func_0011E748, sqrtf), camera target D_008105E0 / D_00810200 = the
//  midpoint + (0, 10, 0), work +0x10 = 0. +4 1 and 2: the camera D_008105D0 /
//  D_008101F0 circles the midpoint at that distance (func_0011E2A8 /
//  func_0011DE90 of 0.008726646 * work +0x10) and rises 0.1 a frame; work
//  +0x10 counts frames; past 380 func_001AEDE0(1, 1) and +4 2; past 600 in +4
//  2 it returns 1.
extern float D_overlay_AREA17_00827520[];
extern float D_overlay_AREA17_00969A80[];
extern float D_00810350[];
extern float D_008101F0[];
extern float D_00810200[];
extern float D_008105D0[];
extern float D_008105E0[];
extern float func_0011E748(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_001AEDE0(int a, int b);

#define m D_overlay_AREA17_00969A80
#define a D_overlay_AREA17_00827520

int func_overlay_AREA17_00823F30(unsigned char *self, unsigned char *rec, unsigned char *work) {
    float t;
    switch (rec[4]) {
    case 0:
        {
            float px;
            float ax;
            float pz;
            float az;
            m[0] = ((ax = a[0]) + (px = D_00810350[0])) / 2.0f;
            m[1] = (a[1] + D_00810350[1]) / 2.0f;
            m[2] = ((az = a[2]) + (pz = D_00810350[2])) / 2.0f;
            m[3] = func_0011E748((ax - px) * (ax - px) + (az - pz) * (az - pz));
        }
        rec[4] = 1;
        *(int *)(work + 0x10) = 0;
        D_00810200[0] = D_008105E0[0] = m[0];
        D_00810200[1] = D_008105E0[1] = 10.0f + m[1];
        D_00810200[2] = D_008105E0[2] = m[2];
        break;
    case 1:
        t = m[0] + m[3] * func_0011E2A8(0.008726646f * *(float *)(work + 0x10));
        D_008105D0[0] = t;
        D_008101F0[0] = t;
        m[1] += 0.1f;
        D_008105D0[1] = m[1];
        D_008101F0[1] = m[1];
        t = m[2] + m[3] * func_0011DE90(0.008726646f * *(float *)(work + 0x10));
        D_008105D0[2] = t;
        D_008101F0[2] = t;
        *(float *)(work + 0x10) += 1.0f;
        if (!(*(float *)(work + 0x10) <= 380.0f)) {
            func_001AEDE0(1, 1);
            rec[4] = 2;
        }
        break;
    case 2:
        t = m[0] + m[3] * func_0011E2A8(0.008726646f * *(float *)(work + 0x10));
        D_008105D0[0] = t;
        D_008101F0[0] = t;
        m[1] += 0.1f;
        D_008105D0[1] = m[1];
        D_008101F0[1] = m[1];
        t = m[2] + m[3] * func_0011DE90(0.008726646f * *(float *)(work + 0x10));
        D_008105D0[2] = t;
        D_008101F0[2] = t;
        *(float *)(work + 0x10) += 1.0f;
        if (!(*(float *)(work + 0x10) <= 600.0f)) {
            return 1;
        }
        break;
    }
    return 0;
}
