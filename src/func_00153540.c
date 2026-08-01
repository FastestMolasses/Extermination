// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: actor state-machine step, dispatched on the 1-byte state at e+4
// (states 0..3).
//   0 init      - spin the direction block at e+0xC0 by 2.5 (func_00103230),
//                 acquire resource handle 0x80000037 into e+0x20, snapshot the
//                 pose e+0xB0..0xBC into the previous-pose slot e+0xA0..0xAC,
//                 advance to state 1.
//   1 integrate - add the per-frame velocity e+0xC0..0xC8 into the pose
//                 e+0xB0..0xB8, then sweep-test the motion with func_0019A570
//                 (prev pose, new pose, mask 7, 0x20); on a hit snap the pose to
//                 D_700031B0 and go to state 2. Independently, when the primary
//                 angle e+0xB0 is falling (velocity e+0xC0 < 0) and has dropped
//                 to <= 20, or is rising and has reached >= 180, also go to
//                 state 2. Re-snapshot the pose, build an identity quaternion in
//                 the scratchpad at 0x700038A0 (0,0,0,1) and rebuild the object
//                 matrix at e+0xD0 from pose + that quaternion.
//   2 fire      - flag the acquired resource (+4 = 2), post effect 0x8000006B at
//                 the pose and effect 0x88A with lifetime 300, advance to 3.
//   3 teardown  - func_001AFC10(e).
//
// The quaternion stores go through the literal scratchpad addresses (absolute
// `lui at,0x7000`) while the two build_trs_matrix arguments take &D_700038A0
// (a %hi/%lo symbol pair) — that split is what the original object encodes.
extern void func_00103230(void *a, void *b, float angle);
extern int func_001EFE00(int a, void *p);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001031E0(void *dst, void *src);
extern void func_001EFD20(int a, void *b);
extern void func_001FBD50(void *e, int a, int b, float f);
extern void func_001AFC10(void *p);
extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);

extern char D_700031B0[];
extern int D_700038A0;

void func_00153540(char *e) {
    unsigned char st;

    st = *(unsigned char *)(e + 4);
    switch (st) {
    case 0:
        *(unsigned char *)(e + 4) = st + 1;
        func_00103230(e + 0xC0, e + 0xC0, 2.5f);
        *(int *)(e + 0x20) = func_001EFE00(0x80000037, e);
        *(float *)(e + 0xA0) = *(float *)(e + 0xB0);
        *(float *)(e + 0xA4) = *(float *)(e + 0xB4);
        *(float *)(e + 0xA8) = *(float *)(e + 0xB8);
        *(float *)(e + 0xAC) = *(float *)(e + 0xBC);
        break;
    case 1:
        *(float *)(e + 0xB0) += *(float *)(e + 0xC0);
        *(float *)(e + 0xB4) += *(float *)(e + 0xC4);
        *(float *)(e + 0xB8) += *(float *)(e + 0xC8);
        if (func_0019A570(e + 0xA0, e + 0xB0, 7, 0x20) != 0) {
            func_001031E0(e + 0xB0, D_700031B0);
            *(unsigned char *)(e + 4) = 2;
        }
        if (*(float *)(e + 0xC0) < 0.0f) {
            if (*(float *)(e + 0xB0) <= 20.0f) {
                *(unsigned char *)(e + 4) = 2;
            }
        } else if (*(float *)(e + 0xB0) >= 180.0f) {
            *(unsigned char *)(e + 4) = 2;
        }
        *(float *)(e + 0xA0) = *(float *)(e + 0xB0);
        *(float *)(e + 0xA4) = *(float *)(e + 0xB4);
        *(float *)(e + 0xA8) = *(float *)(e + 0xB8);
        *(float *)(e + 0xAC) = *(float *)(e + 0xBC);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        build_trs_matrix(e + 0xD0, e + 0xB0, &D_700038A0, &D_700038A0);
        break;
    case 2:
        *(unsigned char *)(e + 4) = st + 1;
        *(unsigned char *)(*(int *)(e + 0x20) + 4) = 2;
        func_001EFD20(0x8000006B, e + 0xB0);
        func_001FBD50(e, 0x88A, 0, 300.0f);
        break;
    case 3:
        func_001AFC10(e);
        break;
    }
}
