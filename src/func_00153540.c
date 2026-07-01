// NEARMISS func_00153540  (vram 0x00153540, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residuals at 99.58%: (1) commutative add.s operand-coloring on the three pose+=velocity adds (target f0+f1 vs mwcc f1+f0 - load order already matched by putting the velocity operand first); (2) the D_700038A0..AC scratchpad quaternion-identity stores use %hi/%lo relocations and AC(1.0f)-first...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.58% (mwcc233). Actor state-machine step, dispatched on the
// 1-byte state at e+4 (states 0..3):
//   0: init - rotate matrix block (func_00103230, 2.5f), acquire a resource
//      handle into e+0x20, snapshot pose e+0xB0.. into e+0xA0.., advance.
//   1: integrate - add velocity e+0xC0.. into pose e+0xB0.., collision probe
//      (func_0019A570); on hit or when the primary angle e+0xB0 leaves
//      [ -inf,20] (falling) / [180,+inf] (rising) advance to 2. Snapshot pose,
//      zero a quaternion scratch (D_700038A0..A8=0, D_700038AC=1.0f), build TRS.
//   2: fire - flag the resource (+4=2), submit two effect calls, advance.
//   3: teardown (func_001AFC10).
// RESIDUAL WALL: (1) commutative add.s operand-coloring on the three
// pose+=velocity adds (target f0+f1 vs mwcc f1+f0); (2) the D_700038A0..AC
// scratchpad stores use %hi/%lo relocations + AC-first order in the target,
// while mwcc materializes the fixed 0x70003xxx addresses as absolute lui and
// keeps A0-first order. Both are compiler/linker artifacts, not source-fixable.
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
extern int D_700038A4;
extern int D_700038A8;
extern int D_700038AC;

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
        *(float *)(e + 0xB0) = *(float *)(e + 0xC0) + *(float *)(e + 0xB0);
        *(float *)(e + 0xB4) = *(float *)(e + 0xC4) + *(float *)(e + 0xB4);
        *(float *)(e + 0xB8) = *(float *)(e + 0xC8) + *(float *)(e + 0xB8);
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
        D_700038AC = 0x3F800000;
        D_700038A0 = 0;
        D_700038A4 = 0;
        D_700038A8 = 0;
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
