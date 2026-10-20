// NEARMISS func_001FC9B0  (vram 0x001FC9B0, 0x54 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// One placement: mwcc 2.3.3 schedules the upper-half address load of D_002821D0 ahead of the D_00275C50 store; the target places it after. Volatile or plain declarations, scalar or array, and store order do not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Clears the 0x9C-byte block D_002821B0, then sets D_00275C50 = D_0026EC10[0],
// D_00275C54 = 0x80, D_002821D0[0] = D_00264D10, D_00275C55 = 0, and returns
// 1.

extern char D_002821B0[0x100];
extern int D_0026EC10[2];
extern char D_00264D10[8];
extern char *volatile D_002821D0[2];
extern volatile int D_00275C50;
extern volatile unsigned char D_00275C54;
extern volatile unsigned char D_00275C55;
extern void func_00121A28(void *dst, int value, int size);

int func_001FC9B0(void) {
    func_00121A28(D_002821B0, 0, 0x9C);
    D_00275C50 = D_0026EC10[0];
    D_00275C54 = 0x80;
    D_002821D0[0] = D_00264D10;
    D_00275C55 = 0;
    return 1;
}
