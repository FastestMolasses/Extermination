// NEARMISS func_00157F60  (vram 0x00157F60, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-order permutation: body 100% correct (all opcodes/order match), sole residual is target $v1/$v0 vs mwcc $a1/$v1 coloring of the loaded mode byte vs store constants. Not the clean-store nop; 2.3.3 does not fix it. Permuter class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 96.75% (mwcc 2.3.3; 991202 = 91.67%). Body/structure 100% correct:
// every instruction matches in opcode and order; the SOLE residual is a
// register-coloring artifact -- the target holds the loaded mode byte in $v1
// and the store-constants in $v0, whereas mwcc puts the byte in $a1 and the
// constants in $v1. Pure regalloc permutation, no idiom fixes it (if-cascade
// reproduces the exact bne 0x38/0x37/0x2C compare chain; a switch is worse).
// PARK: register-allocation-order wall -> permuter territory.
//
// Init/mode-select handler dispatched on the mode byte at arg0+3:
//   0x38 -> B1=0x80, B0=6 ; 0x37 -> B1=0x10, B0=5 ; 0x2C -> B1=0x40, B0=1 ;
//   else -> B1 = (*(u8*)(arg0+0x34)) + 0x80, B0=1.
// All paths then set D_008106D0 = *(int*)(arg0+0x14), zero arg0+0xA/0xB, and
// set arg0+0 = 1.
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern int D_008106D0;

void func_00157F60(char *arg0) {
    int t;

    t = *(unsigned char *)(arg0 + 3);
    if (t == 0x38) {
        D_008106B1 = 0x80;
        D_008106B0 = 6;
        D_008106D0 = *(int *)(arg0 + 0x14);
        *(char *)(arg0 + 0xA) = 0;
        *(char *)(arg0 + 0xB) = 0;
        *(char *)(arg0 + 0) = 1;
        return;
    }
    if (t == 0x37) {
        D_008106B1 = 0x10;
        D_008106B0 = 5;
        D_008106D0 = *(int *)(arg0 + 0x14);
        *(char *)(arg0 + 0xA) = 0;
        *(char *)(arg0 + 0xB) = 0;
        *(char *)(arg0 + 0) = 1;
        return;
    }
    if (t == 0x2C) {
        D_008106B1 = 0x40;
    } else {
        D_008106B1 = *(unsigned char *)(arg0 + 0x34) + 0x80;
    }
    D_008106B0 = 1;
    D_008106D0 = *(int *)(arg0 + 0x14);
    *(char *)(arg0 + 0xA) = 0;
    *(char *)(arg0 + 0xB) = 0;
    *(char *)(arg0 + 0) = 1;
}
