// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.61% NEARMISS).
// Fixes read from the original instructions:
//  - The six floats at 0x008105D0..0x008105E8 are one global record (a position
//    vector followed by a velocity vector), not six separate scalars. As separate
//    globals mwcc proved the stores and loads independent and batched all loads;
//    through one aggregate each update is loaded, added and stored in order, as
//    the original does.
//  - The +17.0 on the field at 0x24 is a compound assignment (operand order).
//
// Copies the global vector at 0x70003B50 to arg0+0x30 and the vector at
// arg1+0xA0 to arg0+0x20, raises the field at arg0+0x24 by 17, copies that
// vector to the global velocity, sets up the scratchpad matrix at 0x70003400
// (func_001029C0, then func_00102C58 with arg0+0x30), passes the point
// (0, 0, arg0[0xC], 1) through func_001026A0 into the global position, adds the
// velocity (plus the float at arg0+0x5C on Y) and copies the result to arg0+0x10.

extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_001026A0(void *, void *, void *);
typedef struct { float pos[4]; float vel[4]; } Mover;
extern Mover D_008105D0;
extern char D_70003400[];
extern char D_70003600[];
extern float D_70003B50;

void func_0018CA90(char *arg0, char *arg1) {
    func_00102948(arg0 + 0x30, &D_70003B50);
    func_00102948(arg0 + 0x20, arg1 + 0xA0);
    *(float *)(arg0 + 0x24) += 17.0f;
    func_00102948(D_008105D0.vel, arg0 + 0x20);
    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, arg0 + 0x30);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = *(float *)(arg0 + 0xC);
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_008105D0.pos, D_70003400, D_70003600);
    D_008105D0.pos[0] += D_008105D0.vel[0];
    D_008105D0.pos[1] += D_008105D0.vel[1] + *(float *)(arg0 + 0x5C);
    D_008105D0.pos[2] += D_008105D0.vel[2];
    func_00102948(arg0 + 0x10, D_008105D0.pos);
}
