// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS (resolved — see docs/FINDINGS.md "ENGINE FRAME ANATOMY"):
// game-task sub-state machine (func_001ACEC0 -> HERE). Dispatches on byte +9
// of the current task slot (slot ptr in scratchpad 0x70003B6C) via the
// 6-entry jr-table at 0x0026DCB0:
//   0: bring-up poll func_001AD360; when done -> state 5, clear +0xA/+0xB,
//      kick func_001AEDB0(0)
//   1: func_001AD4D0 (trampoline to the in-game frame machine anim_frame_top_b)
//   2: func_001AD4E0   3: func_001AD740   4: func_001ADF00
//   5: end poll func_001ADF50; when done -> back to state 1, clear +0xA/+0xB
//
// BYTE-IDENTICAL under mwcc 2.3.3 (the pinned 991202 build mis-fills the delay
// slots, 92.3%). This carried a "NEARMISS 99.85% — jr-table external-dispatch
// wall" note claiming the original consolidated jump tables into an external
// rodata TU, making mwcc's local @13 table a permanent reloc mismatch. That
// wall was disproven in s85: a switch's jump table is .rodata of the SAME
// translation unit, and normalize_asm() now appends it to the target, so both
// sides carry a local table. Re-measured s86: 100.0%. The note was stale, not
// the code — nothing here changed.

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
