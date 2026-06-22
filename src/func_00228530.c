// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Calls the 6-arg helper func_001FEC20 with the two ints at +0x48/+0x4C of the
// object, the table pointer &D_002738E0, 0, the literal 1, and &D_00821500.
// On a non-negative result, stores a status byte at +3: 1 when the result is 0,
// 2 otherwise. CodeWarrior duplicates the store across both branch arms (b to a
// shared epilogue with the store in the delay slot, plus a second store), which
// the 991202 build merges into a single tail store (75.6%). mwcc 2.3.3
// (mwcps2-2.3.3-000906) reproduces the duplicated-store branch layout
// byte-identically. Verified objdiff 100% vs build/expected/func_00228530.o.
extern int func_001FEC20(int, int, void *, int, int, void *);
extern int D_002738E0;
extern int D_00821500;

void func_00228530(signed char *arg0) {
    int ret;

    ret = func_001FEC20(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), &D_002738E0, 0, 1, &D_00821500);
    if (ret >= 0) {
        if (ret == 0) {
            *(signed char *)(arg0 + 3) = 1;
        } else {
            *(signed char *)(arg0 + 3) = 2;
        }
    }
}
