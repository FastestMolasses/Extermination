// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Zero-init of a global state block, with two -1 sentinels at the end.
// mwcc 2.3.3 schedules `li v1,-1` between the lui/sw of D_00264E34, matching
// the target; the pinned 991202 build does not. D_00264E30/34/38 are
// large-data (lui/%lo) globals; D_00275xxx are small-data (gp-rel). Source
// store order is interleaved (D_00264E38 after D_00275C58/5C) to match the
// target instruction scheduling exactly.
extern int D_00264E30[2];
extern int D_00264E34[2];
extern int D_00264E38[2];
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C60;
extern int D_00275C64;
extern int D_00275C68;
extern int D_00275C6C;
extern int D_00275840;
extern int D_00275844;

void func_001FE8D0(void) {
    D_00264E30[0] = 0;
    D_00264E34[0] = 0;
    D_00275C58 = 0;
    D_00275C5C = 0;
    D_00264E38[0] = 0;
    D_00275C60 = 0;
    D_00275C64 = 0;
    D_00275C68 = 0;
    D_00275C6C = 0;
    D_00275840 = -1;
    D_00275844 = -1;
}
