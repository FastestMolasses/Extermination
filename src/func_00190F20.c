// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame area-transition trigger. Only runs when the global gate
// D_008106B8 == 0. In area 0x12: if the actor's +0xA0 distance/height field
// (arg1) is <= 285.0f, signals event 0xE via func_001B0C60(0xE,0,1) and sets
// actor state byte +6 = 7. In area 0xE: if hard-addr gate *(0x70003B8D)==0 and
// the bounds test func_001B1EA0(0, &arg1[0xA0], &D_0024A4B0, 4) passes,
// signals event 0x12 via func_001B0C60(0x12,0,0) and likewise sets +6 = 7.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202, which
// lowered the nested branches with a differently-filled delay slot. 2.3.3
// matches CW exactly. objdiff 100% vs build/expected/func_00190F20.o.
extern void func_001B0C60(int, int, int);
extern int func_001B1EA0(int, float *, void *, int);
extern char D_0024A4B0;
extern unsigned char D_008106B8;
extern unsigned char D_00810700;

void func_00190F20(char *arg0, char *arg1) {
    if (D_008106B8 == 0) {
        if (D_00810700 == 0x12) {
            if (*(float *)(arg1 + 0xA0) <= 285.0f) {
                func_001B0C60(0xE, 0, 1);
                *(char *)(arg0 + 6) = 7;
            }
        } else if (D_00810700 == 0xE) {
            if (*(unsigned char *)0x70003B8D == 0) {
                if (func_001B1EA0(0, (float *)(arg1 + 0xA0), &D_0024A4B0, 4) != 0) {
                    func_001B0C60(0x12, 0, 0);
                    *(char *)(arg0 + 6) = 7;
                }
            }
        }
    }
}
