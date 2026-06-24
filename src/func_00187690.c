// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Publishes a draw/effect request into a global state block, then builds and
// dispatches it. Copies two source quadwords (*arg1, *arg2) into the global
// blocks D_00248800/D_00248810; stashes the three float params into the
// global scalar slots (D_002487E8 = f12; D_00248820 = D_00248824 = f13;
// D_00248830 = D_00248834 = f14); stores arg3 into D_00248860 and the 64-bit
// arg4 (passed in $t0) into D_00248850. Builds a local Vec4 sp30 = {0, 0,
// f12, 1.0f}, runs it through the 4x4 matrix*vector helper func_001026A0
// in-place (a0 == a2 == &sp30, matrix from arg0), acquires a handle via
// func_001CCF70(&sp30), initialises a 0x60 work block sp40 with
// func_001CFA60(&sp40, arg0, 1.0f, 0.1f) (object initialiser: seeds
// +0x44..+0x54, then four-quadword copy of arg0 over its head), and finally
// dispatches func_001CFBE0(handle, 1, &D_002487E0, &sp40, 1).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// 991202 build's scheduler reorders the independent global float/int stores
// and swaps the s0/s1 saved-register assignment, capping it at 81.7%. The
// 2.3.3 build reproduces CW 2.3.1's exact schedule and register choice, so
// this readable C is byte-identical. Verified objdiff 100% vs
// build/expected/func_00187690.o.
//
// Matching notes (so the shape is reproducible):
//   - The scalar globals are declared `volatile` to pin CW's store order at
//     -O4 (without it mwcc sinks the float stores below the int stores).
//   - The two quadword globals are written through an explicit
//     `*(uint128 *)&D_xxxx = *src;` cast so mwcc materialises the address in
//     a GPR (lui/addiu;sq 0(reg)) exactly as CW does, instead of folding
//     %lo into the sq via $at. This cast form also restores the correct
//     arg0->s0 / result->s1 saved-register allocation.
//   - func_001CFBE0 takes a 5th arg in $t0 (also 1); declared in the proto.

typedef unsigned __int128 uint128;
extern void func_001026A0(void *a0, void *a1, void *a2);
extern int func_001CCF70(void *a0);
extern void func_001CFA60(void *obj, void *src, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern unsigned char D_002487E0;
extern volatile float D_002487E8;
extern uint128 D_00248800;
extern uint128 D_00248810;
extern volatile float D_00248820;
extern volatile float D_00248824;
extern volatile float D_00248830;
extern volatile float D_00248834;
extern volatile long long D_00248850;
extern volatile int D_00248860;

struct Vec4 { int x; int y; float z; int w; };
struct Blk60 { unsigned char b[0x60]; };

void func_00187690(void *arg0, uint128 *arg1, uint128 *arg2, int arg3, long long arg4, float f12, float f13, float f14) {
    struct Vec4 sp30;
    struct Blk60 sp40;
    void *p;
    int v0;

    p = arg0;
    *(uint128 *)&D_00248800 = *arg1;
    *(uint128 *)&D_00248810 = *arg2;
    D_002487E8 = f12;
    D_00248820 = f13;
    D_00248824 = f13;
    D_00248830 = f14;
    D_00248834 = f14;
    sp30.x = 0;
    D_00248860 = arg3;
    sp30.y = 0;
    sp30.z = f12;
    D_00248850 = arg4;
    sp30.w = 0x3F800000;
    func_001026A0(&sp30, p, &sp30);
    v0 = func_001CCF70(&sp30);
    func_001CFA60(&sp40, p, 1.0f, 0.1f);
    func_001CFBE0(v0, 1, &D_002487E0, &sp40, 1);
}
