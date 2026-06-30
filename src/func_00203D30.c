// NEARMISS func_00203D30  (vram 0x00203D30, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class regalloc / frame-size: target keeps 5 callee-saved regs (s0-s4 = arg1/arg2/start/len/clamped, frame 0x70) vs mwcc 4 (frame 0x60); cascades into different stack-slot offsets for the 4 out-vars and shuffled temp coloring. Body/logic fully correct (sltu clamp matches).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS func_00203D30 (vram 0x00203D30, 0x128 bytes) — readable decompilation,
// NOT byte-identical (best 78.08% via mwcc 2.3.3; 991202 75.58%).
//
// SEMANTICS: ring-buffer / region copy clamp+dispatch.
//   start  = arg1[2];                         (buffer base/read pos)
//   clamped = (arg2 + *(int*)(arg2+0x50008)) - start;   (bytes available)
//   if ((unsigned)arg1[3] < clamped) clamped = arg1[3]; (clamp to requested len)
//   func_00206B00(D_002DF740, &a,&b,&c,&d) fills 4 region descriptors (out-params);
//   r = func_00203F40(8-arg block-copy dispatcher) with the two 28-bit-masked
//       |0x20000000 addresses (a,c), b, d, start, clamped, arg2, arg1[3]-clamped.
//   if (r>0 && func_00206BA0(D_002DF740, arg1[0x10..], arg1[0x18..], a)==0)
//       func_00204080(D_00273450);          (notify/commit failure path)
//   func_00206B10(D_002DF740, r);            (release)
//   return r>0 ? 1 : 0;
//
// WALL (permuter-class regalloc / frame-size): body/structure fully recovered
// (sltu clamp sense now matches). Sole residuals: the target keeps FIVE values
// in callee-saved regs (s0..s4 = arg1/arg2/start/len/clamped) for a 0x70 frame,
// while mwcc CSEs to FOUR saved regs (0x60 frame); this cascades into different
// stack-slot offsets for the four func_00206B00 out-vars and shuffled temp
// coloring around the dsll32/dsrl32 |0x20000000 setup. Pure register-allocation
// / spill-layout divergence — inverse-CSE inlining and address-form reshapes
// tried without closing the frame-size gap.
extern char D_002DF740[];
extern char D_00273450[];
int func_00206B00();
int func_00203F40();
int func_00206BA0();
int func_00206B10();
int func_00204080();

int func_00203D30(int *arg1, char *arg2) {
    unsigned int clamped;
    int start;
    int sp6C;
    int sp68;
    int sp64;
    int sp60;
    int r;

    start = arg1[2];
    clamped = ((int)arg2 + *(int *)(arg2 + 0x50008)) - start;
    if ((unsigned int)arg1[3] < clamped) {
        clamped = arg1[3];
    }
    func_00206B00(D_002DF740, &sp60, &sp68, &sp64, &sp6C);
    r = func_00203F40((sp60 & 0xFFFFFFF) | 0x20000000, sp68,
                      (sp64 & 0xFFFFFFF) | 0x20000000, sp6C,
                      start, clamped, arg2, arg1[3] - clamped);
    if (r > 0) {
        if (func_00206BA0(D_002DF740, *(long long *)((char *)arg1 + 0x10),
                          *(long long *)((char *)arg1 + 0x18), sp60) == 0) {
            func_00204080(D_00273450);
        }
    }
    func_00206B10(D_002DF740, r);
    if (r > 0) {
        return 1;
    }
    return 0;
}
