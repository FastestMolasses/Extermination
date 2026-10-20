// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame update for an entity e whose state block st sits at e+0x1F0,
// gated on the scratchpad mode byte 0x70003B8D.
// Mode 1: if func_001B2140(e) reports the entity active and e[4] is nonzero,
// call the entity's callback at e+0x4C, then return. Modes 2 and 3: return.
// Modes 0, 4 and any other value: clear the e+0x52 short, set e[0x5E] = 1 and
// run the per-kind handler for e[4] with (e, st): 0 func_0013D490,
// 1 func_0013D600, 2 func_00140690, 3 func_00140C00 (kind 3 returns right
// after its handler). Then decrement each nonzero st counter at +0x70, +0x76,
// +0x72 (shorts) and +0x8B, +0x87, +0x88 (signed bytes), clear the
// e+0x54 short, call func_001B5360(e) when st[0x8E] is set, and finish with
// func_001B0D80(e).
//
// Byte-matched (objdiff 100.0 with mwcc 2.3.3; 991202 reaches 97.0). Matching
// key: the mode switch lists case 0 and case 4 explicitly (each a bare break),
// which gives the target's descending compare chain; without them mwcc packs
// the cases into a shorter chain. The call site leaves st in a1 for
// func_00140C00 as well, so it is declared with two parameters here; the
// callee ignores that argument (src/func_00140C00.c defines it with one
// parameter, and the original never reads a1).
extern int func_001B2140(char *e);
extern void func_0013D490(char *e, char *st);
extern void func_0013D600(char *e, char *st);
extern void func_00140690(char *e, char *st);
extern void func_00140C00(char *e, char *st);
extern void func_001B5360(char *e);
extern int func_001B0D80(char *e);

void func_0013D2D0(char *e) {
    char *st = e + 0x1F0;

    switch (*(unsigned char *)0x70003B8D) {
    case 0:
        break;
    case 1:
        if (func_001B2140(e) == 0) return;
        if (*(unsigned char *)(e + 4) == 0) return;
        (*(void (**)(char *))(e + 0x4C))(e);
        return;
    case 2:
    case 3:
        return;
    case 4:
        break;
    }

    *(short *)(e + 0x52) = 0;
    *(unsigned char *)(e + 0x5E) = 1;
    switch (*(unsigned char *)(e + 4)) {
    case 0:
        func_0013D490(e, st);
        break;
    case 1:
        func_0013D600(e, st);
        break;
    case 2:
        func_00140690(e, st);
        break;
    case 3:
        func_00140C00(e, st);
        return;
    }
    if (*(short *)(st + 0x70) != 0) *(short *)(st + 0x70) -= 1;
    if (*(short *)(st + 0x76) != 0) *(short *)(st + 0x76) -= 1;
    if (*(signed char *)(st + 0x8B) != 0) *(signed char *)(st + 0x8B) -= 1;
    if (*(signed char *)(st + 0x87) != 0) *(signed char *)(st + 0x87) -= 1;
    if (*(short *)(st + 0x72) != 0) *(short *)(st + 0x72) -= 1;
    if (*(signed char *)(st + 0x88) != 0) *(signed char *)(st + 0x88) -= 1;
    *(short *)(e + 0x54) = 0;
    if (*(signed char *)(st + 0x8E) != 0) func_001B5360(e);
    func_001B0D80(e);
}
