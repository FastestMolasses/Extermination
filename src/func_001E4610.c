// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// func_001E4610 (vram 0x001E4610, 0x3D4 bytes)
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.56% NEARMISS). Fixes read
// from the original instructions:
//  - the state dispatch is one switch: case 0 (init) falls through into case 1 (tick),
//    case 2/3 free the actor via func_001AFC10, every case ends in break (no goto/return).
//  - the size pick on the actor's +0xD is a switch (case 0 -> 50, case 1 -> 30, other
//    kinds leave the fields alone), not an if/else-if chain.
//  - the fade value at 0x70003A20 is computed in place in scratchpad memory (each step
//    re-reads it) with a ternary clamp at 0 and a compound multiply by 24.
//  - the growth of scratch+0x18 is two compound adds on the field itself.
//  - the four size-scaled extents read the size through one pointer to scratch+0x14.
//
// Behaviour: 4-state dispatch (0 = init emitter fields + identity transform + link to the
// other actor's scratch block; 1 = tick; 2/3 = free via func_001AFC10). The ring call passes
// the block's doubleword +0x20 and word +0x1C as func_001CE660's fifth and sixth arguments.

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, void *a2);
extern int func_00128250(float a);
extern void func_001AFC10(unsigned char *handle);
extern void func_001B17A0(unsigned char *self);
extern void func_001CE660(int layer, int tag, void *mtx, void *band,
                         unsigned long long tex0, unsigned int rgba);
extern float D_700038B0;
extern void func_001E4600(void);

void func_001E4610(unsigned char *arg0) {
    unsigned char *other;
    unsigned char *scratch;
    unsigned char *other_scratch;
    unsigned char state;
    float f;

    other = *(unsigned char **)(arg0 + 0x14);
    other_scratch = other + 0x1F0;
    scratch = arg0 + 0x1F0;
    state = *(unsigned char *)(arg0 + 4);

    switch (state) {
    case 0:
        /* Size by the actor's own kind byte +0xD: 1 -> 30, 0 -> 50, any other
         * kind keeps s+0x14 / +0x28 / +0x44 / +0x50 as they are. */
        switch (*(unsigned char *)(arg0 + 0xD)) {
        case 0:
            *(float *)(scratch + 0x14) = 50.0f;
            *(float *)(scratch + 0x28) = 5.0f;
            *(int *)(scratch + 0x44) = 0;
            *(int *)(scratch + 0x50) = 0;
            break;
        case 1:
            *(float *)(scratch + 0x14) = 30.0f;
            *(float *)(scratch + 0x28) = 5.0f;
            *(int *)(scratch + 0x44) = 0;
            *(int *)(scratch + 0x50) = 0;
            break;
        }

        *(float *)(scratch + 0x14) = *(float *)(scratch + 0x14) / 2.0f;
        *(float *)(scratch + 0x18) = 0.0f;
        *(long long *)(scratch + 0x20) = ((long long)0x20041A85LL << 32) | 0x553220D0LL;
        *(int *)(scratch + 0x30) = 0x14;
        *(int *)(scratch + 0x34) = 0;
        *(int *)(scratch + 0x38) = 0x43B40000;

        func_001029C0(arg0 + 0xD0);
        func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);

        *(unsigned char **)(arg0 + 0x30) = other_scratch;
        *(void (**)(void))(arg0 + 0x34) = func_001E4600;
        *(unsigned char *)(arg0 + 0xC) = 0;
        *(unsigned char *)(arg0 + 9) = 0;
        *(unsigned char *)(arg0 + 0) = 1;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fall through */
    case 1:
        f = *(float *)(scratch + 0x18);
        *(float *)0x70003A24 = f;
        if (!(f <= 1.0f)) {
            *(unsigned char *)(arg0 + 0) = 2;
            *(float *)0x70003A20 = *(float *)0x70003A24 - 1.0f;
            *(float *)0x70003A20 = 1.0f - *(float *)0x70003A20 / 0.7f;
            *(float *)0x70003A20 = (*(float *)0x70003A20 < 0.0f) ? 0.0f : *(float *)0x70003A20;
            *(float *)0x70003A20 *= 24.0f;
        } else {
            *(unsigned char *)(arg0 + 0) = 1;
            *(float *)0x70003A20 = 24.0f;
        }

        {
            float *size = (float *)(scratch + 0x14);
            *(float *)(scratch + 0x3C) = *(float *)0x70003A24 * *size;
            *(float *)(scratch + 0x40) = *(float *)0x70003A24 * *size;
            *(float *)(scratch + 0x48) = *(float *)(scratch + 0x28) + *(float *)0x70003A24 * *size;
            *(float *)(scratch + 0x4C) = *(float *)(scratch + 0x28) + *(float *)0x70003A24 * *size;
        }

        *(float *)0x700038B0 = *(float *)(scratch + 0x48);
        *(int *)0x700038B4 = 0x3F800000;
        *(float *)0x700038B8 = *(float *)(scratch + 0x4C);
        *(int *)0x700038BC = 0x3F800000;
        func_001026A0(scratch, arg0 + 0xD0, &D_700038B0);
        func_001028D0(scratch, scratch, arg0 + 0x100);

        {
            float v20 = *(float *)0x70003A20;
            *(float *)0x700038A0 = v20;
            *(float *)0x700038A4 = v20;
            *(float *)0x700038A8 = v20;
            *(int *)0x700038AC = 0;
        }

        *(int *)(scratch + 0x1C) = func_00128250(*(float *)0x700038A0);
        *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038A4) << 8);
        *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038A8) << 0x10);
        *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038AC) << 0x18);

        func_001CE660(0, 2, arg0 + 0xD0, scratch + 0x30,
                      *(unsigned long long *)(scratch + 0x20), *(unsigned int *)(scratch + 0x1C));

        {
            float v;
            *(float *)(scratch + 0x18) += (2.0f - *(float *)(scratch + 0x18)) / 25.0f;
            *(float *)(scratch + 0x18) += 0.001f;
            v = *(float *)(scratch + 0x18);
            if (!(v <= 2.0f)) {
                *(unsigned char *)(arg0 + 4) = 3;
                *(unsigned char *)(arg0 + 0) = 2;
            }
        }

        func_001B17A0(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
