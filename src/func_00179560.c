// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Advances an entity's orientation by its turn rate, rebuilds its TRS matrix, writes
// view params into the D_700038A0[] block, then submits via func_001026A0/func_0019AD00.
extern void build_trs_matrix(float *a, float *b, float *c, float *d);
extern void func_001026A0(void *a, float *b, void *c);
extern float func_0011DE90(float x);
extern float func_0011E2A8(float x);
extern void func_0019AD00(void *a, void *b, unsigned int c);
extern float D_700038A0[4];
extern float D_700038B0[16];

void func_00179560(unsigned char *arg0) {
    float angle;
    *(char *)(arg0 + 0x314) = 0;
    angle = *(float *)(arg0 + 0xC4);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x38) * func_0011DE90(angle);
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) - *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
    build_trs_matrix((float *)(arg0 + 0xD0), (float *)(arg0 + 0xB0), (float *)(arg0 + 0xC0), (float *)(arg0 + 0x60));
    if (*(float *)(arg0 + 0x38) <= 0.0f) {
        *(int *)0x700038A0 = (int)0xC0900000;
        *(int *)0x700038A4 = 0x41200000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
    } else {
        *(int *)0x700038A0 = 0x40900000;
        *(int *)0x700038A4 = 0x41200000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
    }
    func_001026A0(D_700038B0, (float *)(arg0 + 0xD0), D_700038A0);
    func_0019AD00(arg0, D_700038B0, 0x80000006);
}
