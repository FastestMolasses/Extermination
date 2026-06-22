// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Boot/area dispatch keyed on a 16-bit code formed from two adjacent global
// bytes: code = (D_00810700 << 8) | D_00810701. Codes 0xB00..0xE00 take the
// hardware/input-gated path: func_001B0070() returns a status word; if any of
// bits 0x60 are set it calls func_001DE920, otherwise func_001DDE10. All other
// codes (0xF02/0x1100/0x1500 and the default) just call func_001DDE10. Returns
// void.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906), not 991202: the DE920-path early return
// gives that branch its own epilogue with `lq ra` filling the branch delay
// slot, plus the body relies on the clean-store idiom-13 delay-slot nops that
// 991202 fills (wall #13). 233 leaves them -> objdiff 100.0 vs
// build/expected/func_001DDAA0.o. (991202 = 87.76%.)
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int func_001B0070(void);
extern void func_001DE920(void);
extern void func_001DDE10(void);

void func_001DDAA0(void) {
    int v1;

    v1 = (D_00810700 << 8) + D_00810701;
    switch (v1) {
    case 0xB00:
    case 0xC00:
    case 0xD00:
    case 0xE00:
        if (func_001B0070() & 0x60) {
            func_001DE920();
            return;
        }
        func_001DDE10();
        return;
    case 0xF02:
    case 0x1100:
    case 0x1500:
        break;
    }
    func_001DDE10();
}
