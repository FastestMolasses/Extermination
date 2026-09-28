// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00823A90 (splat/link name 00823A50; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: range gate and look-at setup for the O3 owner family (called from
//  0x8240E0 state 2): returns 0 while +0xB8 is in (-585,-545) and +0xB4 in
//  (35,50); otherwise builds a scratchpad look-at matrix from the talk block
//  (+0x1F0 +0x70/+0x80), transforms a1/a2 and stores the resulting yaw at
//  +0x1F0+0xE8, returning 1.
// The three-float copy 0x700038A0..A8 -> 0x700036A0..A8 is written through the
// relocated scratchpad externs (MATCHING_GUIDE idiom-32); the literal-address
// spelling makes mwcc reorder the loads.
// Covers the splat pieces 00823A50, 00823A90 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
#define SPAD(a) (*(float *)(a))
extern float D_700036A0[16];
extern float D_700036E0[16];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern void func_001029C0(void *m);
extern void func_00102718(void *a0, void *a1, void *a2);
extern void func_001027E0(void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *a, void *b);
extern float func_001B1240(void *origin, float x, float z);

int func_overlay_AREA01_00823A50(unsigned char *self, void *a1, void *a2) {
    unsigned char *talk = self + 0x1F0;
    float z = *(float *)(self + 0xB8);
    float x;
    float px;
    float pz;
    float py;
    if (z < -545.0f && z > -585.0f) {
        x = *(float *)(self + 0xB4);
        if (x < 50.0f && x > 35.0f) {
            return 0;
        }
    }
    func_001029C0(D_700036A0);
    SPAD(0x700036C0) = *(float *)(talk + 0x70);
    SPAD(0x700036C4) = *(float *)(talk + 0x74);
    SPAD(0x700036C8) = *(float *)(talk + 0x78);
    SPAD(0x700036B0) = *(float *)(talk + 0x80);
    SPAD(0x700036B4) = *(float *)(talk + 0x84);
    SPAD(0x700036B8) = *(float *)(talk + 0x88);
    SPAD(0x700038B0) = *(float *)(talk + 0x80);
    SPAD(0x700038B4) = *(float *)(talk + 0x84);
    SPAD(0x700038B8) = *(float *)(talk + 0x88);
    SPAD(0x700038BC) = 1.0f;
    SPAD(0x700038C0) = *(float *)(talk + 0x70);
    SPAD(0x700038C4) = *(float *)(talk + 0x74);
    SPAD(0x700038C8) = *(float *)(talk + 0x78);
    SPAD(0x700038CC) = 1.0f;
    func_00102718(D_700038A0, D_700038B0, D_700038C0);
    D_700036A0[0] = D_700038A0[0];
    D_700036A0[1] = D_700038A0[1];
    D_700036A0[2] = D_700038A0[2];
    func_001027E0(D_700036E0, D_700036A0);
    func_001028D0(D_700038B0, a1, a2);
    func_001026A0(D_700038A0, D_700036E0, D_700038B0);
    SPAD(0x700038C0) = 0.0f;
    SPAD(0x700038C4) = 0.0f;
    SPAD(0x700038C8) = 0.0f;
    *(float *)(talk + 0xE8) = func_001B1240(D_700038C0, -SPAD(0x700038A0), -SPAD(0x700038A8));
    return 1;
}
