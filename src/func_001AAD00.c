// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 68.31% NEARMISS).
// Fixes read from the original instructions:
// - Not a scheduler wall: the statement order was wrong. Each list's swap
//   publishes the COUNT first, then the cursor, then resets the cursor and
//   zeroes the live count.
// - The class lists are swapped in descending address order, and the
//   unpublished list (D_00275B54 / D_00275B58) is reset LAST, not first.
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

    D_00275BB4 = D_00275BB8;
    D_00275BAC = D_00275BB0;
    D_00275BB0 = D_0028B020;
    D_00275BB8 = 0;
    D_00275BA4 = D_00275BA8;
    D_00275B9C = D_00275BA0;
    D_00275BA0 = D_0028AFF0;
    D_00275BA8 = 0;
    D_00275B94 = D_00275B98;
    D_00275B8C = D_00275B90;
    D_00275B90 = D_0028AF30;
    D_00275B98 = 0;
    D_00275B84 = D_00275B88;
    D_00275B7C = D_00275B80;
    D_00275B80 = D_0028AE30;
    D_00275B88 = 0;
    D_00275B74 = D_00275B78;
    D_00275B6C = D_00275B70;
    D_00275B70 = D_0028AC30;
    D_00275B78 = 0;
    D_00275B64 = D_00275B68;
    D_00275B5C = D_00275B60;
    D_00275B60 = D_0028AB30;
    D_00275B68 = 0;
    D_00275B58 = 0;
    D_00275B54 = D_0028AAB0;
}
