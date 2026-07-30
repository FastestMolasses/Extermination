// NEARMISS func_001AD250  (vram 0x001AD250, 0x108 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — ONLY residual. Under mwcc233 the emitted instruction stream is identical to the target; the sole diff is the lw's reloc (local @13 vs external jtbl_0026DCB0). NOTE: requires // COMPILER: mwcc233 — the pinned 991202 mwcc caps at 92.3% (mis-fills the t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// game-task sub-state machine (func_001ACEC0 -> HERE). Dispatches on byte +9
// of the current task slot (slot ptr in scratchpad 0x70003B6C) via the
// 6-entry jr-table at 0x0026DCB0:
//   0: bring-up poll func_001AD360; when done -> state 5, clear +0xA/+0xB,
//      kick func_001AEDB0(0)
//   1: func_001AD4D0 (trampoline to the in-game frame machine func_001AE040)
//   2: func_001AD4E0   3: func_001AD740   4: func_001ADF00
//   5: end poll func_001ADF50; when done -> back to state 1, clear +0xA/+0xB
//
// NEARMISS 99.85% — jr-table external-dispatch wall (proven s84): the original
// consolidated all jump tables into an external rodata TU, so mwcc's local @13
// table is a permanent reloc mismatch. Everything else — all six bodies, the
// three beqz+nop delay slots, the jtbl lui/addiu/sll ordering and case 0's
// early-epilogue `b` with the `lq $ra` delay slot — is byte-identical under
// mwcc 2.3.3 (the pinned 991202 build mis-filled the delay slots, 92.3%).

extern int func_001AD360(void);
extern void func_001AD4D0(void);
extern void func_001AD4E0(void);
extern void func_001AD740(void);
extern void func_001ADF00(void);
extern int func_001ADF50(void);
extern void func_001AEDB0(int);

void func_001AD250(void) {
    switch (*(unsigned char *)(*(char *volatile *)0x70003B6C + 0x9)) {
    case 0:
        if (func_001AD360()) {
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0x9) = 5;
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0xA) = 0;
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0xB) = 0;
            func_001AEDB0(0);
        }
        break;
    case 1:
        func_001AD4D0();
        break;
    case 2:
        func_001AD4E0();
        break;
    case 3:
        func_001AD740();
        break;
    case 4:
        func_001ADF00();
        break;
    case 5:
        if (func_001ADF50()) {
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0x9) = 1;
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0xA) = 0;
            *(unsigned char *)(*(char *volatile *)0x70003B6C + 0xB) = 0;
        }
        break;
    }
}
