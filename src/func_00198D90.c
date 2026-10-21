// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 98.8% NEARMISS).
// Camera follow step: copies player + 0xA0 to cam + 0x10 and raises it by 3.0,
// builds the player matrix (D_70003400 from player + 0xC0) and places the
// target cam + 0x20 at the offset {0, 0, 5, 0} (D_70003600) from it. Then on
// the state byte cam + 1: 0 advances, clears +2 and copies both points to
// D_008105E0 / D_008105D0; 1 (player code 0x12 sets +6 = 0xB and state 0)
// eases D_008105E0 toward the target by 0.2 (func_0018C4B0 / func_0018C6A0)
// and copies cam + 0x10 to D_008105D0.
// How it matches: the +3.0 is `+=` (idiom-21) and both cases end in `break`
// (idiom-27). The old header's two "compiler artifacts" were those spellings.

extern void func_001026A0(char *p, float *m, int *v);
extern void func_001028B8(char *a, char *b, char *c);
extern void func_00102948(float *m, char *v);
extern void func_001029C0(float *m);
extern void func_00102C58(float *a, float *b, char *c);
extern void func_0018C4B0(float *m, float a, float b);
extern void func_0018C6A0(char *p, float *m, float a);
extern float D_008105D0;
extern float D_008105E0;
extern float D_70003400;
extern int D_70003600;

void func_00198D90(char *arg0, char *arg1) {
    unsigned char st;

    func_00102948((float *)(arg0 + 0x10), arg1 + 0xA0);
    *(float *)(arg0 + 0x14) += 3.0f;
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, arg1 + 0xC0);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x40A00000;
    *(int *)0x7000360C = 0;
    func_001026A0(arg0 + 0x20, &D_70003400, &D_70003600);
    func_001028B8(arg0 + 0x20, arg0 + 0x20, arg0 + 0x10);
    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        func_00102948(&D_008105E0, arg0 + 0x20);
        func_00102948(&D_008105D0, arg0 + 0x10);
        break;
    case 1:
        if (*(int *)(arg1 + 0x230) == 0x12) {
            *(char *)(arg0 + 6) = 0xB;
            *(unsigned char *)(arg0 + 1) = 0;
        }
        func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 0.2f);
        func_0018C6A0(arg0 + 0x20, &D_008105E0, 0.2f);
        func_00102948(&D_008105D0, arg0 + 0x10);
        break;
    }
}
