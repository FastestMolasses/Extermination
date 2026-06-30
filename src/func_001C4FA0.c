// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Status predicate dispatched on the type byte at arg0+3. For type 1 returns
// (D_00810C87 != 0); for type 7 returns (D_0081075D != 0xFF); for type 5 returns
// (D_0081076D != 0); for type 6 returns (D_00810770 != 0); any other type
// returns 0. mwcc reverses the switch into a 6/5/7/1 descending compare chain.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 69.9%. Key idiom: each case writes the false path as `goto zero;` to a single
// shared `return 0;` label, which forces mwcc to emit a conditional branch to
// the shared return-0 tail (the value 1 falls through from the dispatch
// compare) instead of materialising the boolean with movz/sltu. objdiff 100.0
// vs build/expected/func_001C4FA0.o.
extern unsigned char D_0081075D;
extern unsigned char D_0081076D;
extern unsigned char D_00810770;
extern unsigned char D_00810C87;

int func_001C4FA0(char *arg0) {
    switch (*(unsigned char *)(arg0 + 3)) {
    case 1:
        if (D_00810C87 != 0) { return 1; }
        goto zero;
    case 7:
        if (D_0081075D != 0xFF) { return 1; }
        goto zero;
    case 5:
        if (D_0081076D != 0) { return 1; }
        goto zero;
    case 6:
        if (D_00810770 != 0) { return 1; }
        goto zero;
    default:
    zero:
        return 0;
    }
}
