// NEARMISS func_0014C0B0  (vram 0x0014C0B0, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word
// body, matching wave 2026-10-28). Body-correct: same calls, stores and conditions as the original.
// Remaining differences: the final `arg0[5] < 3` test. The original keeps the compare result in
// v1, branches over an unconditional branch to the exit and stores the two bytes after it; mwcc
// 2.3.3 computes the compare in at and falls through into the stores. Early return, goto,
// if/else with empty arm, switch on arg0[5] or arg0[4], > 2 / <= 2 / int casts, a boolean
// local, and mwcc 991202 / 2.4 all tried (991202: 89.68%).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Leaf, (e, st): a gate that drops the actor into state 9. When the e+0xD
// bit 0x80 is clear, the st+0x74 short is 0, the st+0x38 word is nonzero,
// the e+0x54 short is nonzero, st[0x81] bit 0x2 is set, the mode e[4] is 1 and
// the state e[5] is below 3, it sets e[5] = 9 and e[6] = 0 (sub-step reset).
// Called from func_00147700.
void func_0014C0B0(char *arg0, char *arg1) {
    if (*(unsigned char *)(arg0 + 0xD) & 0x80) return;
    if (*(short *)(arg1 + 0x74) != 0) return;
    if (*(int *)(arg1 + 0x38) == 0) return;
    if (*(short *)(arg0 + 0x54) == 0) return;
    if (!(*(char *)(arg1 + 0x81) & 2)) return;
    if (*(unsigned char *)(arg0 + 4) != 1) return;
    if (*(unsigned char *)(arg0 + 5) < 3) {
        *(unsigned char *)(arg0 + 5) = 9;
        *(unsigned char *)(arg0 + 6) = 0;
    } else {
        return;
    }
}
