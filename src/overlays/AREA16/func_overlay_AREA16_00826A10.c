// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00826A50 (splat/link name 00826A10; overlay code is
// linked 0x40 below where it runs), 0x1CC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Covers the splat pieces 00826A10, 00826A50 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x826A50, self, pos, speed) when |speed| >= 0.5 emits three
//  class 0x8000001D effects (func_001EFD90) at the D_008103DC, D_008103CC and
//  their midpoint with D_008103C0 (height pos[2], rotation (-pi/2, 0, 0, 1))
//  and plays sound 0x926 (speed >= 0) or 0x927.
extern unsigned char *D_008103C0;
extern unsigned char *D_008103CC;
extern unsigned char *D_008103DC;
extern void func_001EFD90(int id, float *pos, float *rot);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);

void func_overlay_AREA16_00826A10(unsigned char *self, float *a1, float f) {
    float p[4];
    float v[4];
    unsigned char *o;
    unsigned char *o2;
    if (f > 0.0f) {
        if (f < 0.5f) {
            return;
        }
    } else if (!(f <= -0.5f)) {
        return;
    }
    v[0] = -1.5707964f;
    v[1] = 0.0f;
    o = D_008103DC;
    v[2] = 0.0f;
    v[3] = 1.0f;
    p[0] = *(float *)(o + 0xC0);
    p[1] = *(float *)(o + 0xC4);
    p[2] = a1[2];
    func_001EFD90(0x8000001D, p, v);
    o = D_008103CC;
    p[0] = *(float *)(o + 0xC0);
    p[1] = *(float *)(o + 0xC4);
    p[2] = a1[2];
    func_001EFD90(0x8000001D, p, v);
    o = D_008103CC;
    o2 = D_008103C0;
    p[0] = (*(float *)(o + 0xC0) + *(float *)(o2 + 0xC0)) / 2.0f;
    p[1] = (*(float *)(o + 0xC4) + *(float *)(o2 + 0xC4)) / 2.0f;
    p[2] = a1[2];
    func_001EFD90(0x8000001D, p, v);
    if (!(f < 0.0f)) {
        func_001FBD50(self, 0x926, 0, 300.0f);
    } else {
        func_001FBD50(self, 0x927, 0, 300.0f);
    }
}
