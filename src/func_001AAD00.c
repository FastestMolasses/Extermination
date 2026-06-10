// INCLUDE_ASM func_001AAD00  (vram 0x001AAD00, 308 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001AAD00.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// BEST READABLE-C ATTEMPT (2026-06-10): 75.13%, wall-blocked on
// SCHEDULER DIVERGENCE (the documented gp-load/store rematerialization
// wall family). The 9-call hook sequence matches; the 26-statement
// gp-rel double-buffer swap block does not: both compilers software-
// pipeline the independent lw/lh+sw/sh pairs, but with different
// lookahead/latency models, so the emission interleave differs over
// ~45 instructions. Source statement order DOES steer it coarsely -
// per-list [publish-cursor, publish-count, reset-cursor, reset-count]
// scored 75.1% vs 64.6% for [pC,rC,pN,rN] and 61.2% for cursors-then-
// counts (searched all 6 legal within-group orders; ties: pC,pN,rN,rC).
// Pinning exact order via volatile globals + explicit temps FAILS:
// mwcc reorders accesses to DISTINCT volatile objects (55.7%) and
// allocates different temp registers. Without an instruction-level
// scheduling control there is no path from C to CW's exact interleave.
//
// Attempt source (compiles, logic verified against the .s):
//
// // Frame CLOSE-OUT: runs the per-system end-of-frame hooks, then
// // double-buffer-swaps the per-class actor pointer lists that
// // func_001B1B70/func_001B1DE0 filled during the frame. For each list
// // the swap publishes last frame's cursor and count (so consumers always
// // read a complete list), resets the cursor to its static base, and
// // zeroes the live count: interactive D_00275B60/B68 -> D_00275B5C/B64
// // (base D_0028AB30), class-1 D_00275B70/B78 -> D_00275B6C/B74 (base
// // D_0028AC30), class-4 D_00275B80/B88 -> D_00275B7C/B84 (base
// // D_0028AE30), class-7 D_00275B90/B98 -> D_00275B8C/B94 (base
// // D_0028AF30), class-0xD D_00275BA0/BA8 -> D_00275B9C/BA4 (base
// // D_0028AFF0), and D_00275BB0/BB8 -> D_00275BAC/BB4 (base D_0028B020).
// // The class-2/0xA area cursor D_00275B54 resets to D_0028AAB0 and its
// // count D_00275B58 just zeroes (no published copy). The player actor
// // D_008102B0 gets its own hooks (func_001A9F60 / func_001A8BE0).
// extern void func_001A9D20(void);
// extern void func_001A8DA0(void);
// extern void func_001A9F60(unsigned char *player);
// extern void func_001AA140(void);
// extern void func_001A7870(void);
// extern void func_001A8BE0(unsigned char *player);
// extern void func_001A9000(void);
// extern void func_001A97B0(void);
// extern void func_001A9B10(void);
//
// extern unsigned char D_008102B0[];  // player actor
//
// extern unsigned char D_0028B020[];  // list bases (static arrays)
// extern unsigned char D_0028AFF0[];
// extern unsigned char D_0028AF30[];
// extern unsigned char D_0028AE30[];
// extern unsigned char D_0028AC30[];
// extern unsigned char D_0028AB30[];
// extern unsigned char D_0028AAB0[];
//
// extern unsigned char *D_00275BB0;  // live cursors
// extern unsigned char *D_00275BA0;
// extern unsigned char *D_00275B90;
// extern unsigned char *D_00275B80;
// extern unsigned char *D_00275B70;
// extern unsigned char *D_00275B60;
// extern unsigned char *D_00275B54;
// extern short D_00275BB8;           // live counts
// extern short D_00275BA8;
// extern short D_00275B98;
// extern short D_00275B88;
// extern short D_00275B78;
// extern short D_00275B68;
// extern short D_00275B58;
// extern unsigned char *D_00275BAC;  // published cursors
// extern unsigned char *D_00275B9C;
// extern unsigned char *D_00275B8C;
// extern unsigned char *D_00275B7C;
// extern unsigned char *D_00275B6C;
// extern unsigned char *D_00275B5C;
// extern short D_00275BB4;           // published counts
// extern short D_00275BA4;
// extern short D_00275B94;
// extern short D_00275B84;
// extern short D_00275B74;
// extern short D_00275B64;
//
// void func_001AAD00(void) {
//     func_001A9D20();
//     func_001A8DA0();
//     func_001A9F60(D_008102B0);
//     func_001AA140();
//     func_001A7870();
//     func_001A8BE0(D_008102B0);
//     func_001A9000();
//     func_001A97B0();
//     func_001A9B10();
//
//     D_00275B58 = 0;
//     D_00275BAC = D_00275BB0;
//     D_00275BB4 = D_00275BB8;
//     D_00275BB0 = D_0028B020;
//     D_00275BB8 = 0;
//     D_00275B9C = D_00275BA0;
//     D_00275BA4 = D_00275BA8;
//     D_00275BA0 = D_0028AFF0;
//     D_00275BA8 = 0;
//     D_00275B8C = D_00275B90;
//     D_00275B94 = D_00275B98;
//     D_00275B90 = D_0028AF30;
//     D_00275B98 = 0;
//     D_00275B7C = D_00275B80;
//     D_00275B84 = D_00275B88;
//     D_00275B80 = D_0028AE30;
//     D_00275B88 = 0;
//     D_00275B6C = D_00275B70;
//     D_00275B74 = D_00275B78;
//     D_00275B70 = D_0028AC30;
//     D_00275B78 = 0;
//     D_00275B5C = D_00275B60;
//     D_00275B64 = D_00275B68;
//     D_00275B60 = D_0028AB30;
//     D_00275B68 = 0;
//     D_00275B54 = D_0028AAB0;
// }
