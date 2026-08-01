// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Animation/clip tick. Seeds the shared transform scratch D_700038A0 from the
// actor's +0xB0 block (func_001031E0), bumps the scratch's +4 float by 4.0f, then
// runs func_001760C0(actor, &D_700038A0, 1, 18.0f). If that returns nonzero the
// tick is done -> return 3. Otherwise advance the clip (func_00180420), copy the
// actor's current pos floats (+0x290 -> 0x700038A0, +0x298 -> 0x700038A8), set the
// +4 scratch float to 18.0f + actor->+0xB4, and return func_00180300(actor,
// &D_700038A0, actor->+0xD byte). The three direct float fields are accessed at
// their absolute HW-scratch addresses 0x700038A0/A4/A8 (the target does NOT relocate
// them through the D_700038A0 symbol -- only the pointer args do).
extern void func_001031E0(void *a, void *b);
extern int func_001760C0(void *a, void *b, int c, float f);
extern void func_00180420(char *p);
extern int func_00180300(char *a, char *b, unsigned char c);
extern float D_700038A0[];

int func_00180460(char *arg0) {
    func_001031E0(D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 += 4.0f;
    if (func_001760C0(arg0, D_700038A0, 1, 18.0f) == 0) {
        func_00180420(arg0);
        *(float *)0x700038A0 = *(float *)(arg0 + 0x290);
        *(float *)0x700038A8 = *(float *)(arg0 + 0x298);
        *(float *)0x700038A4 = 18.0f + *(float *)(arg0 + 0xB4);
        return func_00180300(arg0, (char *)D_700038A0, *(unsigned char *)(arg0 + 0xD));
    }
    return 3;
}
