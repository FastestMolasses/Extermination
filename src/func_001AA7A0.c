// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Proximity/zone test between a global subject (*D_00275B40, fields at +0xC0..)
// and the param a1 (fields at +0xB0..). Returns 1 and sets a1->[0x36]=1 when a1
// is within a 25-unit squared horizontal radius and inside a vertical band
// [B4-45 .. B4]; otherwise returns 0. a0 unused.
extern int D_00275B40;

int func_001AA7A0(int a0, int a1)
{
    int v0 = *(int *)D_00275B40;
    float dx = *(float *)(v0 + 0xC0) - *(float *)(a1 + 0xB0);
    float dz = *(float *)(v0 + 0xC8) - *(float *)(a1 + 0xB8);
    if (dx * dx + dz * dz > 25.0f) return 0;
    if (*(float *)(v0 + 0xC4) > *(float *)(a1 + 0xB4)) return 0;
    if (*(float *)(v0 + 0xC4) < *(float *)(a1 + 0xB4) - 45.0f) return 0;
    *(short *)(a1 + 0x36) = 1;
    return 1;
}
