// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.80% NEARMISS).
// Fixes read from the original instructions:
// - each arm of the 0x1B5 / 0x1B2 test makes its own trailing func_00106AB0 call
//   (the original keeps two copies; one shared call after the if/else merged them).
// - the table-index clamp is a separate assignment `idx = (idx > 0xA) ? 0 : idx;`
//   before the call; the clamp inside the subscript scaled the index before selecting.
// - externs follow the callee definitions in src/ (func_00106AB0 and func_00106830
//   return void).
//
// Command loop: after an initial func_00106AB0, keep reading a 32-bit code with
// func_001066F8(0x20). Code 0x1B5: consume it (func_00106830), read an index with
// func_00106948(4), clamp indices above 10 to 0 and call that handler-table entry. Code 0x1B2:
// consume it only. Each handled code is followed by func_00106AB0; any other code ends
// the loop.

extern void func_00106AB0(void);
extern void func_00106830(int);
extern int func_00106948(int);
extern int func_001066F8(int);
extern void (*ftab_002411A8[])(void);

void func_00106D80(void) {
    int v1;
    func_00106AB0();
    while ((v1 = func_001066F8(0x20)) == 0x1B5 || v1 == 0x1B2) {
        if (v1 == 0x1B5) {
            unsigned int idx;
            func_00106830(0x20);
            idx = func_00106948(4);
            idx = (idx > 0xA) ? 0 : idx;
            ftab_002411A8[idx]();
            func_00106AB0();
        } else {
            func_00106830(0x20);
            func_00106AB0();
        }
    }
}
