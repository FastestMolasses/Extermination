// NEARMISS func_001E7570  (vram 0x001E7570, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class FP register coloring: float temp `v` and the 0.2/0.05/0.015 literal scratch regs color to ft0/fv0f (target) vs ft2/fv1 (mwcc) across the neg/mul/sub/add chain; plus one switch-dispatch delay-slot nop the target leaves that mwcc fills. Body/logic fully correct.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS func_001E7570 (vram 0x001E7570, 0x128 bytes) — readable decompilation,
// NOT byte-identical (best 98.11% via mwcc 2.3.3; 991202 87.30%).
//
// SEMANTICS: per-frame update of a weapon/effect object at +4 (a state byte).
//   state 0: init -> set state=1, +0x60(float)=1.0, +0x64(float)=64.0,
//            func_001D2830(9,1), func_001DEE80(9,&D_0026E990); fall through to 1.
//   state 1: func_001DEEC0(9, (int)(+0x64)); func_001DF9D0(0.2 * +0x60);
//            +0x64 *= 0.98; v = +0x60; v += (0.05*-v) - 0.015; store +0x60;
//            if (v <= 0) state = 3.
//   states 2 & 3: func_001D2830(9,0); func_001AFC10(self) (teardown).
//
// WALL (permuter-class FP register coloring): body/structure fully recovered.
// Sole residuals (98.11%): (1) the float temp `v`/literal scratch regs color
// to ft0/fv0f in the target but ft2/fv1 under mwcc across the
// neg/mul/sub/add chain — pure FP regalloc, not fixable by C reshape
// (operand-order and split-temp variants tried). (2) one delay-slot nop the
// target leaves in the `beq a0,2` switch-dispatch slot that mwcc fills.
extern int float_to_int(float);
extern void func_001AFC10(void *);
extern void func_001D2830(int, int);
extern void func_001DEE80(int, void *);
extern void func_001DEEC0(int, int);
extern void func_001DF9D0(float);
extern unsigned char D_0026E990;

void func_001E7570(unsigned char *p) {
    float v;
    float nv;

    switch (p[4]) {
    case 0:
        p[4] = 1;
        *(float *)(p + 0x60) = 1.0f;
        *(float *)(p + 0x64) = 64.0f;
        func_001D2830(9, 1);
        func_001DEE80(9, &D_0026E990);
    case 1:
        func_001DEEC0(9, float_to_int(*(float *)(p + 0x64)));
        func_001DF9D0(0.2f * *(float *)(p + 0x60));
        *(float *)(p + 0x64) = *(float *)(p + 0x64) * 0.98f;
        v = *(float *)(p + 0x60);
        nv = v + ((0.05f * -v) - 0.015f);
        *(float *)(p + 0x60) = nv;
        if (nv <= 0.0f) {
            p[4] = 3;
        }
        return;
    case 2:
    case 3:
        func_001D2830(9, 0);
        func_001AFC10(p);
        break;
    }
}
