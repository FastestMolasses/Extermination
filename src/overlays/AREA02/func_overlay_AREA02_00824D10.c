// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824D50 (splat/link name 00824D10; overlay
// code is linked 0x40 below where it runs), 0x244 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00824D10, 00824D50 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: owner = +0x1C. Four points at +0x2B0 = the local points
//  0x827630..0x827660 through the owner's bone +0x110[0] + 0x90. When the
//  owner's (+0xB0, +0xB8) is within 70 of (D_00810360, D_00810368): returns
//  2 if D_00810350 lies between point 0 x and point 1 x,
//  func_001B1EA0(0, D_00810350, 0x8275B0, 4) == 1, -pi/2 < D_00810374 < 0
//  and D_0081050C == 3; returns 1 for points 3/2, quad 0x8275F0 and
//  -pi < D_00810374 <= -pi/2 (same D_0081050C test); else 0.
extern float D_00810350[];
extern float D_00810360;
extern float D_00810368;
extern float D_00810374;
extern unsigned char D_0081050C;
extern char D_overlay_AREA02_00827630[];
extern char D_overlay_AREA02_00827640[];
extern char D_overlay_AREA02_00827650[];
extern char D_overlay_AREA02_00827660[];
extern char D_overlay_AREA02_008275B0[];
extern char D_overlay_AREA02_008275F0[];
extern void func_001026A0(void *dst, void *a, void *b);
extern int func_001B1EA0(int a, void *b, void *c, int d);

int func_overlay_AREA02_00824D10(unsigned char *self) {
    unsigned char *o = *(unsigned char **)(self + 0x1C);
    float *v = (float *)(self + 0x2B0);
    float dx;
    float dz;

    func_001026A0(v, *(char **)(o + 0x110) + 0x90, D_overlay_AREA02_00827630);
    func_001026A0(v + 4, *(char **)(o + 0x110) + 0x90, D_overlay_AREA02_00827640);
    func_001026A0(v + 8, *(char **)(o + 0x110) + 0x90, D_overlay_AREA02_00827650);
    func_001026A0(v + 12, *(char **)(o + 0x110) + 0x90, D_overlay_AREA02_00827660);
    dx = *(float *)(o + 0xB0) - D_00810360;
    dz = *(float *)(o + 0xB8) - D_00810368;
    if (dx * dx + dz * dz < 4900.0f) {
        if (D_00810350[0] < v[4] && D_00810350[0] > v[0]
            && func_001B1EA0(0, D_00810350, D_overlay_AREA02_008275B0, 4) == 1
            && -1.5707964f < D_00810374 && D_00810374 < 0.0f && D_0081050C == 3) {
            return 2;
        }
        if (D_00810350[0] < v[8] && D_00810350[0] > v[12]
            && func_001B1EA0(0, D_00810350, D_overlay_AREA02_008275F0, 4) == 1
            && -3.1415927f < D_00810374 && D_00810374 <= -1.5707964f && D_0081050C == 3) {
            return 1;
        }
    }
    return 0;
}
