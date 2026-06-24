// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Screen color-fade / blend setup. Copies two 16-byte constant templates
// (D_00253500, D_00253510) into stack qword buffers, then builds a 4-int color
// from the actor struct at arg0: col[i] = float_to_int((float)(0xFF - field_i)
// * arg1) for the three byte/word channels at +0,+4,+8, with col[3] = 1.
// Issues func_001D1F80(1,0,3), then func_001D64A0(1,&buf0,&buf1,col,0x40), then
// func_001D2040(1,1). Returns void.
//
// mwcc 2.3.3 byte-matches; the pinned 991202 build caps at 77.2%. KEY: the two
// constant copies must be STRUCT assignments (a named 4-int struct type), not
// u128/value loads -- the struct copy makes mwcc materialize the source address
// (lui/addiu + lq 0(reg)) and the dest stack address (addiu sp,0x40) as
// separate pointers, exactly matching CW. A plain u128 assignment folds to
// lui+lq %lo() and mis-schedules. Local declaration order col,buf0,buf1 pins
// the stack offsets (col@0x30, buf0@0x40, buf1@0x50). Verified objdiff 100.0%.
typedef struct { int a, b, c, d; } Vec4;
extern int float_to_int(float);
extern Vec4 D_00253500;
extern Vec4 D_00253510;
extern void func_001D1F80(int a, int b, int c);
extern void func_001D64A0(int a, void *b, void *c, void *d, int e);
extern void func_001D2040(int a, int b);

void func_001DFE70(int arg0, float arg1) {
    int *s0 = (int *)arg0;
    int col[4];
    Vec4 buf0;
    Vec4 buf1;

    buf0 = D_00253500;
    buf1 = D_00253510;

    col[0] = float_to_int((float)(0xFF - s0[0]) * arg1);
    col[1] = float_to_int((float)(0xFF - s0[1]) * arg1);
    col[2] = float_to_int((float)(0xFF - s0[2]) * arg1);
    col[3] = 1;

    func_001D1F80(1, 0, 3);
    func_001D64A0(1, &buf0, &buf1, &col[0], 0x40);
    func_001D2040(1, 1);
}
