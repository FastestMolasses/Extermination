// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.75% NEARMISS).
// Fixes read from the original instructions:
//  - The function is not void: it returns int 1 on every path. The constant 1
//    (also stored to arg0+0) is left in the return register, which is why the
//    original colours the store constants in $v0; declaring it void made mwcc
//    pick other registers (the "register permutation" was a wrong prototype).
//
// Init/mode-select handler dispatched (via a table; no direct callers) on the
// mode byte at arg0+3:
//   0x38 -> B1=0x80, B0=6 ; 0x37 -> B1=0x10, B0=5 ; 0x2C -> B1=0x40, B0=1 ;
//   else -> B1 = (*(u8*)(arg0+0x34)) + 0x80, B0=1.
// All paths then set D_008106D0 = *(int*)(arg0+0x14), zero arg0+0xA/0xB,
// set arg0+0 = 1 and return 1.
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern int D_008106D0;

int func_00157F60(char *arg0) {
    int t;

    t = *(unsigned char *)(arg0 + 3);
    if (t == 0x38) {
        D_008106B1 = 0x80;
        D_008106B0 = 6;
        D_008106D0 = *(int *)(arg0 + 0x14);
        *(char *)(arg0 + 0xA) = 0;
        *(char *)(arg0 + 0xB) = 0;
        *(char *)(arg0 + 0) = 1;
        return 1;
    }
    if (t == 0x37) {
        D_008106B1 = 0x10;
        D_008106B0 = 5;
        D_008106D0 = *(int *)(arg0 + 0x14);
        *(char *)(arg0 + 0xA) = 0;
        *(char *)(arg0 + 0xB) = 0;
        *(char *)(arg0 + 0) = 1;
        return 1;
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
    return 1;
}
