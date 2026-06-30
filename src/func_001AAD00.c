// NEARMISS func_001AAD00  (vram 0x001AAD00, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 68.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Documented scheduler-divergence wall, reproduced exactly. All instructions (every lw/lh load + sw/sh store) are present and correct; the 9 leading hook calls match 1:1. Residual is a pure instruction-SCHEDULING interleave in the ~45-instruction gp-rel double-buffer swap block: CW and mwcc softwar...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 75.13% (mwcc 991202; 2.3.3 scores 68.3%, worse). Body fully
// correct: the 9 end-of-frame hooks match 1:1; residual is a pure
// instruction-SCHEDULING divergence in the gp-rel double-buffer swap
// block. CW and mwcc both software-pipeline the independent lw/lh load
// + sw/sh store pairs but with different lookahead/latency models, so
// the emission interleave differs over ~45 instructions. Source order
// steers it coarsely (this [publish-cursor, publish-count, reset-cursor,
// reset-count] order is the best of all 6 legal within-group orders);
// pinning the exact interleave via volatiles/temps FAILS (mwcc reorders
// distinct-object accesses). No source-level path to CW's exact schedule.
//
// Frame CLOSE-OUT: runs the per-system end-of-frame hooks, then double-
// buffer-swaps the per-class actor pointer lists that func_001B1B70/
// func_001B1DE0 filled during the frame. For each list the swap publishes
// last frame's cursor and count (so consumers always read a complete
// list), resets the cursor to its static base, and zeroes the live count.
extern void func_001A9D20(void);
extern void func_001A8DA0(void);
extern void func_001A9F60(unsigned char *player);
extern void func_001AA140(void);
extern void func_001A7870(void);
extern void func_001A8BE0(unsigned char *player);
extern void func_001A9000(void);
extern void func_001A97B0(void);
extern void func_001A9B10(void);

extern unsigned char D_008102B0[];   // player actor

extern unsigned char D_0028B020[];   // list bases (static arrays)
extern unsigned char D_0028AFF0[];
extern unsigned char D_0028AF30[];
extern unsigned char D_0028AE30[];
extern unsigned char D_0028AC30[];
extern unsigned char D_0028AB30[];
extern unsigned char D_0028AAB0[];

extern unsigned char *D_00275BB0;     // live cursors
extern unsigned char *D_00275BA0;
extern unsigned char *D_00275B90;
extern unsigned char *D_00275B80;
extern unsigned char *D_00275B70;
extern unsigned char *D_00275B60;
extern unsigned char *D_00275B54;
extern short D_00275BB8;              // live counts
extern short D_00275BA8;
extern short D_00275B98;
extern short D_00275B88;
extern short D_00275B78;
extern short D_00275B68;
extern short D_00275B58;
extern unsigned char *D_00275BAC;     // published cursors
extern unsigned char *D_00275B9C;
extern unsigned char *D_00275B8C;
extern unsigned char *D_00275B7C;
extern unsigned char *D_00275B6C;
extern unsigned char *D_00275B5C;
extern short D_00275BB4;              // published counts
extern short D_00275BA4;
extern short D_00275B94;
extern short D_00275B84;
extern short D_00275B74;
extern short D_00275B64;

void func_001AAD00(void) {
    func_001A9D20();
    func_001A8DA0();
    func_001A9F60(D_008102B0);
    func_001AA140();
    func_001A7870();
    func_001A8BE0(D_008102B0);
    func_001A9000();
    func_001A97B0();
    func_001A9B10();

    D_00275B58 = 0;
    D_00275BAC = D_00275BB0;
    D_00275BB4 = D_00275BB8;
    D_00275BB0 = D_0028B020;
    D_00275BB8 = 0;
    D_00275B9C = D_00275BA0;
    D_00275BA4 = D_00275BA8;
    D_00275BA0 = D_0028AFF0;
    D_00275BA8 = 0;
    D_00275B8C = D_00275B90;
    D_00275B94 = D_00275B98;
    D_00275B90 = D_0028AF30;
    D_00275B98 = 0;
    D_00275B7C = D_00275B80;
    D_00275B84 = D_00275B88;
    D_00275B80 = D_0028AE30;
    D_00275B88 = 0;
    D_00275B6C = D_00275B70;
    D_00275B74 = D_00275B78;
    D_00275B70 = D_0028AC30;
    D_00275B78 = 0;
    D_00275B5C = D_00275B60;
    D_00275B64 = D_00275B68;
    D_00275B60 = D_0028AB30;
    D_00275B68 = 0;
    D_00275B54 = D_0028AAB0;
}
