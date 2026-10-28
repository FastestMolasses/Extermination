// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003680
// SPAD: 0x70003604

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.40% NEARMISS).
// Fixes read from the original instructions:
//  - arg0 is a record pointer: the x and z floats at +0xB0/+0xB8 are read as
//    fields of one base (the original copies arg0 once and loads base+disp).
//  - arg1 is a signed char (the original sign-extends the saved copy for the
//    switch), so the switch is on arg1 directly.
//  - The scratchpad words 0x70003680 and 0x70003604 are relocated externs
//    (idiom-32, opted in with // SPAD:): the random offset is stored to
//    D_70003680 and then added into D_70003604 by re-reading D_70003680, which
//    lets the original load D_70003604 ahead of that store.
//
// Builds a scratchpad vector from the object's x/z (func_001B1240 result into
// 0x70003614), transforms it through the matrices at 0x70003400 / D_00810360,
// adds a random 0..5 height offset, and fires event 0x80000006/22/23 chosen by
// arg1 (0, 1, 2) with the vector at 0x70003600 and the block at 0x70003610.

extern float func_001B1240(void *, float, float);
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern int func_001026A0(void *, void *, void *);
extern int func_001028B8(void *, void *, void *);
extern int func_00122BB8(void);
extern int func_001EFD90(int, void *, void *);
extern char D_00810360;
extern char D_70003400;
extern char D_70003600;
extern char D_70003610;
extern float D_70003680;
extern float D_70003604;

/* Caller's object: a position vector at +0xB0 (x, y, z). */
typedef struct { char pad[0xB0]; float x; float y; float z; } Obj;

void func_001B55E0(Obj *arg0, signed char arg1) {
    int code;
    int sel;

    *(int *)0x70003618 = 0;
    *(float *)0x70003614 = 0.0f;
    *(int *)0x70003610 = 0;
    *(int *)0x7000361C = 0x3F800000;
    *(float *)0x70003614 = func_001B1240(&D_00810360, arg0->x, arg0->z);
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, *(float *)0x70003614);
    *(int *)0x70003600 = 0;
    D_70003604 = 0.0f;
    *(int *)0x70003608 = 0x40000000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, &D_70003400, &D_70003600);
    func_001028B8(&D_70003600, &D_00810360, &D_70003600);
    sel = arg1;
    D_70003680 = 5.0f * ((float)((func_00122BB8() >> 5) & 0xFF) / 255.0f);
    D_70003604 += D_70003680;
    switch (sel) {
    case 0:
        code = 0x80000006;
        break;
    case 1:
        code = 0x80000022;
        break;
    case 2:
        code = 0x80000023;
        break;
    }
    func_001EFD90(code, &D_70003600, &D_70003610);
}
