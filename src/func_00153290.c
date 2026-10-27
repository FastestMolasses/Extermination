// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.31% NEARMISS).
// Fixes read from the original instructions:
//  - Wrong body: a NULL from func_001AFA90 returns from the whole function
//    (the second loop runs only when all 6 spawns succeed, with no i >= 6
//    test), and both copy_qw4 calls copy the bone matrix into D_700036A0,
//    not into the actor / D_700038A0.
//  - The scratchpad words 0x700038A0..0x700038BC are relocated externs
//    (idiom-32, one per address, opted in with // SPAD:). A4..AC and B0 are
//    volatile: the original keeps the first loop's store order and the
//    dead zero store to 0x700038B0 before its final value; A0 and B4..BC are plain,
//    which lets the D_00275B40 load hoist above the second loop's stores.
//  - The stored int patterns are floats: 0x3F800000 = 1.0f and
//    0xBFC6D3F2 = -1.55334306f (about -89 degrees in radians).
//  - The last two calls pass the volatile arrays without a cast (a cast
//    changes the argument-setup order).

// Twin particle-spawn loop. First loop (6 iterations; returns early if
// func_001AFA90(2) returns NULL) allocates a class-2 actor per entry of
// D_00246730[5][4], sets model byte=8 and behavior=func_00153540, copies its
// position (elems 1..3) into +0xB0..0xB8 with w=1.0, copies the bone
// matrix at D_00275B40[idx]+0x90 (idx = elem 0) into D_700036A0 and
// transforms the position by it (func_001026A0), then builds a small
// rotation matrix in D_700036A0 (identity, rotate by a fixed -0.235619456f
// angle, then by func_001B1470(arg0->0xC4 + elem 4)), packs a randomized
// alpha word into D_700038A0 (0.75 + 0.35 * rand-byte/255), and transforms
// it into the actor's +0xC0 vector via func_001026A0. A second loop (4
// iterations over D_002467B0[5][4]) builds a similar per-entry position +
// angle pair in D_700038A0/D_700038B0 and spawns two more objects via
// func_0014D1E0 and func_001EFD90.

extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001029C0(void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern int func_00122BB8(void);
extern void func_0014D1E0(void *, void *, int);
extern char *func_001AFA90(int);
extern float func_001B1470(float);
extern void func_001EFD90(int, void *, void *);

extern float D_00246730[5][4];
extern float D_002467B0[5][4];
extern char **D_00275B40;
extern int D_700036A0[4];
extern float D_700038A0[4];
extern volatile float D_700038A4[4];
extern volatile float D_700038A8[4];
extern volatile float D_700038AC[4];
extern volatile float D_700038B0[4];
extern float D_700038B4[4];
extern float D_700038B8[4];
extern float D_700038BC[4];
extern void func_00153540(char *);

void func_00153290(char *arg0) {
    int i;
    char *obj;
    float *p;
    int idx;

    p = D_00246730[0];
    for (i = 0; i < 6; i++) {
        obj = func_001AFA90(2);
        if (obj == 0) {
            return;
        }
        obj[3] = 8;
        *(void **)(obj + 0x10) = (void *)func_00153540;
        *(float *)(obj + 0xB0) = p[1];
        *(float *)(obj + 0xB4) = p[2];
        *(float *)(obj + 0xB8) = p[3];
        *(int *)(obj + 0xBC) = 0x3F800000;

        idx = *(int *)p;
        copy_qw4(D_700036A0, (char *)D_00275B40[idx] + 0x90);
        func_001026A0(obj + 0xB0, D_700036A0, obj + 0xB0);

        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, -0.235619455575943f);
        func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + p[4]));

        D_700038A0[0] = 0.0f;
        D_700038A4[0] = 0.0f;
        D_700038A8[0] = 0.75f + (0.35f * ((float)((func_00122BB8() >> 0xF) & 0xFF) / 255.0f));
        D_700038AC[0] = 1.0f;

        func_001026A0(obj + 0xC0, D_700036A0, D_700038A0);

        p += 5;
    }

    p = D_002467B0[0];
    for (i = 0; i < 4; i++) {
        D_700038A0[0] = p[1];
        D_700038A4[0] = p[2];
        D_700038A8[0] = p[3];
        D_700038AC[0] = 1.0f;

        idx = *(int *)p;
        copy_qw4(D_700036A0, (char *)D_00275B40[idx] + 0x90);
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);

        D_700038B8[0] = 0.0f;
        D_700038B4[0] = 0.0f;
        D_700038B0[0] = 0.0f;
        D_700038BC[0] = 1.0f;
        D_700038B0[0] = -1.55334306f;
        D_700038B4[0] = func_001B1470(*(float *)(arg0 + 0xC4) + p[4]);

        func_0014D1E0(D_700038A0, D_700038B0, 1);
        func_001EFD90(3, D_700038A0, D_700038B0);

        p += 5;
    }
}
