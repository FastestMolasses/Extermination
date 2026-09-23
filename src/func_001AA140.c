// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B88

//
// Pairwise entity-collision/processing sweep. Copies the
// active count D_00275B98 into scratchpad short *0x70003B88 and, if >= 2,
// walks the entity-pointer array D_00275B90. For each outer entity e0 whose
// flags qualify -- (e0[2] & 0x1F)==2, e0[3]==0, (*(int*)(e0+0x2D4) & 0xF)==0,
// e0[0]!=2 -- it copies the remaining outer count into the inner scratchpad
// short *0x70003B86 and walks the rest of the array; each inner entity e1
// passing the same four-part test triggers func_001AA000(e0, e1, e0+0x1F0,
// e1+0x1F0). Both counters are decremented per iteration. 0x70003B88/86 are
// EE scratchpad shorts; D_00275B90/B98 are gp-relative (sdatathreshold 8).
//
// MATCH NOTE (m2-matching lane, 93.31% -> 100.0%): both counters are
// relocated scratchpad externs (idiom-32), over-declared as arrays so
// -sdatathreshold 8 keeps them absolute; 0x70003B88 is opted in per file by
// `// SPAD: 0x70003B88`. The inner counter 0x70003B86 is VOLATILE (reloaded at
// each access, as the target does) and the outer 0x70003B88 is not (the
// target seeds the inner counter from the value its loop test loaded). The
// `>= 2` guard is written `> 1` (idiom-28: slti into $at). The literal
// spelling let mwcc speculate `lui at,0x7000` into two bnez slots.
extern void func_001AA000(char *a, char *b, char *c, char *d);
extern char **D_00275B90;
extern short D_00275B98;
extern volatile short D_70003B86[8];
extern short D_70003B88[8];

void func_001AA140(void) {
    char *s0;
    char **s1;
    char **s2;
    char *s3;
    char *a1p;
    char *a3p;
    short v1;

    D_70003B88[0] = D_00275B98;
    v1 = D_70003B88[0];
    if (v1 > 1) {
        s1 = D_00275B90;
        D_70003B88[0] = (short)(v1 - 1);
        while (D_70003B88[0] != 0) {
            s0 = *s1;
            s1 += 1;
            s3 = s0 + 0x1F0;
            if ((*(unsigned char *)(s0 + 2) & 0x1F) == 2 &&
                *(unsigned char *)(s0 + 3) == 0 &&
                (*(int *)(s3 + 0xE4) & 0xF) == 0 &&
                *(unsigned char *)(s0 + 0) != 2) {
                D_70003B86[0] = D_70003B88[0];
                s2 = s1;
                while (D_70003B86[0] != 0) {
                    a1p = *s2;
                    s2 += 1;
                    D_70003B86[0] = (short)(D_70003B86[0] - 1);
                    a3p = a1p + 0x1F0;
                    if ((*(unsigned char *)(a1p + 2) & 0x1F) == 2 &&
                        *(unsigned char *)(a1p + 3) == 0 &&
                        (*(int *)(a3p + 0xE4) & 0xF) == 0 &&
                        *(unsigned char *)(a1p + 0) != 2) {
                        func_001AA000(s0, a1p, s3, a3p);
                    }
                }
            }
            D_70003B88[0] = (short)(D_70003B88[0] - 1);
        }
    }
}
