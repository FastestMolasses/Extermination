// NEARMISS func_001CEEE0  (vram 0x001CEEE0, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// prologue instruction-scheduling + 128-bit-load register coloring: logic fully recovered and the whole call sequence (s0/s1, all jals, 0xA0 frame) matches; only the two lq/sq global-copy stores in the prologue differ in interleave and in which regs the qwords land in (target reuses arg ptr regs a1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 83.44% (mwcc 2.3.3; 991202 = 72.81%). Logic 100% recovered
// (matches m2c decode). The entire call sequence from func_001026A0 onward --
// the s0/s1 callee-save coloring, all four jal sites, frame size 0xA0 -- is
// byte-identical. Residual is confined to the prologue store block: the
// target interleaves the two 128-bit global copies (lq/sq) and their address
// materialization a different way and reuses the incoming arg pointer
// registers (lq a1,0(a1) / lq a2,0(a2)) for the loaded qwords, whereas mwcc
// schedules paddub s0 earlier and colors the loads into fresh regs (a0/a5).
// Instruction-scheduling + lq-destination register coloring -- permuter class.
//
// Keys that landed: (1) sp40 is a >=0x60-byte struct (func_001CFA60 writes
// through +0x0..+0x54), which alone fixes the 0xA0 frame; (2) writing the two
// 128-bit global copies through a pointer cast `*(uint128*)&D_002513xx`
// forces the register-indirect addiu+sq form AND flips arg0->$s0 / result->$s1
// to match the target.
//
// Stashes the two incoming 128-bit vectors to D_00251300/0x310, the three
// incoming floats to D_002512E8/0x320/0x324/0x330/0x334, arg3 to D_00251360,
// builds a {0,0,fparg0,1.0f} quad on the stack (sp30) and runs func_001026A0
// on it, gets a handle via func_001CCF70(sp30), fills sp40 via
// func_001CFA60(sp40, self, 1.0f, 0.1f), then func_001CFBE0(handle, 1,
// &D_002512E0, sp40, 1).
typedef unsigned __int128 uint128;

extern void func_001026A0(void *a0, int a1, void *a2);
extern int func_001CCF70(void *a0);
extern void func_001CFA60(void *obj, int src, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);

extern int D_002512E0;
extern float D_002512E8;
extern uint128 D_00251300;
extern uint128 D_00251310;
extern float D_00251320;
extern float D_00251324;
extern float D_00251330;
extern float D_00251334;
extern int D_00251360;

void func_001CEEE0(int arg0, uint128 *arg1, uint128 *arg2, int arg3, float fparg0, float fparg1, float fparg2) {
    float sp30[4];
    char sp40[0x60];
    int r;

    *(uint128 *)&D_00251300 = *arg1;
    *(uint128 *)&D_00251310 = *arg2;
    D_002512E8 = fparg0;
    D_00251320 = fparg1;
    D_00251324 = fparg1;
    D_00251330 = fparg2;
    *(int *)&sp30[0] = 0;
    D_00251334 = fparg2;
    *(int *)&sp30[1] = 0;
    sp30[2] = fparg0;
    D_00251360 = arg3;
    *(int *)&sp30[3] = 0x3F800000;
    func_001026A0(sp30, arg0, sp30);
    r = func_001CCF70(sp30);
    func_001CFA60(sp40, arg0, 1.0f, 0.1f);
    func_001CFBE0(r, 1, &D_002512E0, sp40, 1);
}
