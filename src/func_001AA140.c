// NEARMISS func_001AA140  (vram 0x001AA140, 0x15C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + instruction-scheduling permutation (NOT the clean-store nop). Body 100% logically correct at 93.3% (233). Residuals: (1) initial counter colored a0 vs target v1 + slti temp v1 vs at; (2) two bnez delay slots target leaves nop while mwcc hoists lui at,0x7000; (3) inner-loop add...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS (body-correct; residual is register-coloring + instruction
// scheduling only). Pairwise entity-collision/processing sweep. Copies the
// active count D_00275B98 into scratchpad short *0x70003B88 and, if >= 2,
// walks the entity-pointer array D_00275B90. For each outer entity e0 whose
// flags qualify -- (e0[2] & 0x1F)==2, e0[3]==0, (*(int*)(e0+0x2D4) & 0xF)==0,
// e0[0]!=2 -- it copies the remaining outer count into the inner scratchpad
// short *0x70003B86 and walks the rest of the array; each inner entity e1
// passing the same four-part test triggers func_001AA000(e0, e1, e0+0x1F0,
// e1+0x1F0). Both counters are decremented per iteration. 0x70003B88/86 are
// EE scratchpad shorts; D_00275B90/B98 are gp-relative (sdatathreshold 8).
//
// Best: mwcc 2.3.3 = 93.3%, 991202 = 89.0%. Logic fully recovered; the sole
// residual is a compiler artifact, NOT the clean-store nop: (1) register
// coloring of the initial counter (target keeps it in v1 with the slti result
// in at; mwcc colors it a0 with slti->v1), and (2) instruction scheduling --
// the target leaves nop in two bnez delay slots and orders the inner
// addiu/lui pair differently from mwcc's hoist. Permuter territory; parked.
extern void func_001AA000(char *a, char *b, char *c, char *d);
extern char **D_00275B90;
extern short D_00275B98;

void func_001AA140(void) {
    char *s0;
    char **s1;
    char **s2;
    char *s3;
    char *a1p;
    char *a3p;
    short v1;

    *(short *)0x70003B88 = D_00275B98;
    v1 = *(short *)0x70003B88;
    if (v1 >= 2) {
        s1 = D_00275B90;
        *(short *)0x70003B88 = (short)(v1 - 1);
        while (*(short *)0x70003B88 != 0) {
            s0 = *s1;
            s1 += 1;
            s3 = s0 + 0x1F0;
            if ((*(unsigned char *)(s0 + 2) & 0x1F) == 2 &&
                *(unsigned char *)(s0 + 3) == 0 &&
                (*(int *)(s3 + 0xE4) & 0xF) == 0 &&
                *(unsigned char *)(s0 + 0) != 2) {
                *(short *)0x70003B86 = *(short *)0x70003B88;
                s2 = s1;
                while (*(short *)0x70003B86 != 0) {
                    a1p = *s2;
                    s2 += 1;
                    *(short *)0x70003B86 = (short)(*(short *)0x70003B86 - 1);
                    a3p = a1p + 0x1F0;
                    if ((*(unsigned char *)(a1p + 2) & 0x1F) == 2 &&
                        *(unsigned char *)(a1p + 3) == 0 &&
                        (*(int *)(a3p + 0xE4) & 0xF) == 0 &&
                        *(unsigned char *)(a1p + 0) != 2) {
                        func_001AA000(s0, a1p, s3, a3p);
                    }
                }
            }
            *(short *)0x70003B88 = (short)(*(short *)0x70003B88 - 1);
        }
    }
}
