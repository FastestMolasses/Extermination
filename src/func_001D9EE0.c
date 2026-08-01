// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Renders the "scaled overlay" pass for an actor: pushes a GS/DMA tag into the
// display-list cursor (D_00275670 word 0x10 -> byte+3 = 0x30, word+4 = &D_00817E20,
// half+0 = 0x10, cursor += 0x10), then builds two matrices in the uncached scratch
// buffer D_70003AC0. The first is the identity template D_00817F20 with the float
// argument written onto the diagonal (elements 0 and 5) and 1.0f at 10/15; the
// second is a fixed screen/projection matrix. Each is concatenated into
// D_70003AC0 via func_001026D0. It then saves the actor's 0x80 vector, zeroes it,
// draws through func_001C7420(self, 0x3F5, 0), restores the vector, kicks
// func_001D4740 with the D_0028A490 entry selected by self+0x96, restores the
// previous scratch matrix and finishes with func_001D1F20(0).
//
// Keys to the match: D_00275670 is a gp-rel pointer global (needs
// -sdatathreshold 4) while D_00817E20 / D_00817F20 / D_70003AC0 must be arrays so
// they stay absolute hi/lo at that threshold; the matrices are union{float;int}[16]
// so the field stores fold to direct off(sp); and the three display-list-cursor
// stores must re-read *(int *)(st + 0x10) inline (a `dl` temp inverts the $7/$8
// colouring of the first three instructions). Verified objdiff 100% with
// mwcc 2.3.3 (mwcps2-2.3.3-000906); mwcc24 also reaches 100%, the pinned 991202
// build only 80.0%.
extern char *D_00275670;         // gp-rel: word 0x10 holds the display-list cursor
extern int D_00817E20[16];
extern int D_00817F20[16];
extern char D_70003AC0[64];      // uncached scratch matrix buffer
extern int D_0028A490[];

extern void copy_qw4(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_001C7420(int a, int b, int c);
extern void func_001D4740(int a);
extern void func_001D1F20(int a);

typedef struct { union { float f; int i; } e[16]; } Mtx;

void func_001D9EE0(float arg0, char *arg1) {
    float farg;
    char sp30[0x40];   // sp+0x30
    Mtx sp70;          // sp+0x70
    Mtx spB0;          // sp+0xB0
    char spF0[0x10];   // sp+0xF0
    char *self = arg1;
    char *st = (char *)D_00275670;

    farg = arg0;

    *(char *)(*(int *)(st + 0x10) + 3) = 0x30;
    *(int *)(*(int *)(st + 0x10) + 4) = (int)D_00817E20;
    *(short *)(*(int *)(st + 0x10)) = 0x10;
    *(int *)(st + 0x10) = *(int *)(st + 0x10) + 0x10;

    copy_qw4(sp30, D_70003AC0);
    copy_qw4(D_70003AC0, D_00817F20);
    func_001029C0(&sp70);

    sp70.e[5].f = farg;
    sp70.e[0].f = farg;
    sp70.e[10].i = 0x3F800000;
    sp70.e[15].i = 0x3F800000;
    func_001026D0(D_70003AC0, &sp70, D_70003AC0);

    spB0.e[0].i  = 0x3F800000;
    spB0.e[4].i  = 0;
    spB0.e[8].i  = 0;
    spB0.e[12].i = 0x45000000;
    spB0.e[1].i  = 0;
    spB0.e[5].i  = 0x3F800000;
    spB0.e[9].i  = 0;
    spB0.e[13].i = 0x45000000;
    spB0.e[2].i  = 0;
    spB0.e[6].i  = 0;
    spB0.e[10].i = 0x3F664CB3;
    spB0.e[14].i = 0x49CCCCCC;
    spB0.e[3].i  = 0;
    spB0.e[7].i  = 0;
    spB0.e[11].i = 0;
    spB0.e[15].i = 0x3F800000;
    func_001026D0(D_70003AC0, &spB0, D_70003AC0);

    func_00102948(spF0, self + 0x80);
    *(int *)(self + 0x80) = 0;
    *(int *)(self + 0x84) = 0;
    *(int *)(self + 0x88) = 0;
    func_001C7420((int)self, 0x3F5, 0);
    func_00102948(self + 0x80, spF0);
    func_001D4740(D_0028A490[*(short *)(self + 0x96)]);
    copy_qw4(D_70003AC0, sp30);
    func_001D1F20(0);
}
