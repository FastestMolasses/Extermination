// NEARMISS func_00176180  (vram 0x00176180, 0x158 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single FP-load scheduling/coloring artifact: target hoists the *0x700031C8 lwc1 into the swc1 0x60(sp) store-latency shadow using a distinct FP register; mwcc emits it one slot later reusing one FP reg. Load-shadow-fill-hoist scheduling wall (the func_001C84D0/anim-decode family), not the clean-s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 97.2% (mwcc233) / 90.5% (991202). Body/structure fully recovered;
// SOLE residual is an FP-load schedule/coloring artifact: the target hoists the
// 'lwc1 *0x700031C8' load into the latency shadow of the 'swc1 0x60(sp)' store
// (between the +C0 add and the writeback), keeping the two adds in distinct FP
// regs. mwcc emits the load one slot later reusing one FP reg. This is the
// address/load shadow-fill-hoist scheduling wall family -- NOT the clean-store
// nop, so 2.3.3 does not close it. arg1 (a1) is an unused hidden middle param;
// arg0 is in a0, arg2 in a2. Updates a per-character pose: builds vec3 v70 from
// matrix/transform helpers (4.5f scaled), calls func_0019AFE0; on success adds
// scratchpad offsets C0/C8 before writing arg0->0xB0 / 0xB8; sets flag 0x700031F0
// when arg0->4==1 && arg0->5==0x1D.

extern float D_00281B50;

extern float func_00102738(float *a, float *b);
extern void func_0011E748(float x);
extern void func_00102760(float *out, float *src);
extern void func_00102948(float *out, char *src);
extern void func_001028B8(float *out, float *a, float *b);
extern int func_0019AFE0(char *a, char *b, float *v, int n);

void func_00176180(char *arg0, int arg1, char *arg2) {
    float a40[4];
    float a50[4];
    float a60[4];
    float v70[4];

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x3C) {
        func_0011E748(func_00102738(&D_00281B50, &D_00281B50));
        func_00102760(a40, &D_00281B50);
        func_00102948(a50, arg0 + 0xB0);
        func_001028B8(a60, a50, &D_00281B50);
        v70[0] = a60[0] + 4.5f * a40[0];
        v70[2] = a60[2] + 4.5f * a40[2];
        v70[1] = *(float *)(arg2 + 4);
        if (func_0019AFE0(arg0, arg2, v70, 6) != 0) {
            a60[0] = a60[0] + *(float *)0x700031C0;
            a60[2] = a60[2] + *(float *)0x700031C8;
            *(float *)(arg0 + 0xB0) = a60[0];
            *(float *)(arg0 + 0xB8) = a60[2];
        } else {
            *(float *)(arg0 + 0xB0) = a60[0];
            *(float *)(arg0 + 0xB8) = a60[2];
        }
        if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 0x1D) {
            *(int *)0x700031F0 = 1;
        }
    }
}
