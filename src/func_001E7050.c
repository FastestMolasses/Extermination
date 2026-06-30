// NEARMISS func_001E7050  (vram 0x001E7050, 0x2B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-order (coloring) permutation: target colors src->s1 and the 0x1F0 table base->s3 (arg0->s4, case-0 counter->s0, inner counter->s2); mwcc colors src->s0/table->s1, shifting every sN by one and pulling two add/increment ops into adjacent delay slots. Declaration-order and split-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 97.3% (mwcc 2.3.3; 991202 = 86.0%). Body fully recovered; all
// instructions present and in order. Sole residual is a callee-saved register
// COLORING permutation: the target assigns src(arg0+0x24)->s1 and the 0x1F0
// table base->s3 (with arg0->s4, the case-0 counter->s0, the inner counter->s2),
// while mwcc colors src->s0 / table->s1, which shifts every subsequent sN by one
// and drags two add/increment ops into adjacent delay slots. Declaration-order
// and split-counter nudges do not move mwcc's coloring (it allocs by live range,
// not source order). Pure register-allocation-order -> permuter territory.
//
// Per-state spawn/emitter driver, dispatched on the state byte at arg0+4.
//   state 0: seed an 8-entry table at arg0+0x1F0: each slot's +0 int is a random
//     index (func_00122BB8() % src->numbones at src+0xC), and the +0x20 / +0x40
//     floats are random unit values (rng / 2^31). Then clear arg0+0x250 and set
//     state = 1.
//   state 1: if the source (arg0+0x24) has already finished (its +4 == 3),
//     latch state = 3 and bail. Otherwise advance the local clock arg0+0xB4 by
//     8.0, allocate a handle via func_001CD070(arg0+0xB0, 0x30); if it is valid
//     (!= 0xFFFFFF), compute a spawn count func_001CD2B0(8,16,384,128) into the
//     scratch float at 0x70003A20, and while that count is nonzero emit 8
//     particles: for each slot of the 0x1F0 table, reset the identity matrix
//     D_700036A0, build the bone matrix into D_700036D0 from the slot's bone,
//     fill a stack descriptor via func_001CFAE0 and submit it with
//     func_001CFBE0; then step the slot phase (+0x40) by 0.2, wrapping past 1.4
//     by re-rolling the slot index.
//   states 2 and 3: hand off to func_001AFC10(arg0).
extern int func_00102948(char *dst, char *src);
extern void func_001029C0(char *m);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern int func_001CD070(char *p, int n);
extern float func_001CD2B0(float a, float b, float c, float d);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a, int b, void *c, void *d, int e);
extern char D_00255320[];
extern char D_700036A0[];
extern char D_700036D0[];

void func_001E7050(char *arg0) {
    char *src = *(char **)(arg0 + 0x24);
    unsigned char st = *(unsigned char *)(arg0 + 4);
    char *tbl = arg0 + 0x1F0;
    char *p;
    int i;
    int j;
    int h;
    float ph;
    char sp60[0x60];

    switch (st) {
    case 0:
        i = 0;
        p = tbl;
        do {
            *(int *)p = func_00122BB8() % *(unsigned char *)(src + 0xC);
            *(float *)(p + 0x20) = (float)func_00122BB8() / 2147483648.0f;
            i++;
            *(float *)(p + 0x40) = (float)func_00122BB8() / 2147483648.0f;
            p += 4;
        } while (i < 8);
        *(int *)(tbl + 0x60) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        break;
    case 1:
        if (*(unsigned char *)(src + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        func_00102948(arg0 + 0xB0, src + 0x100);
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 8.0f;
        h = func_001CD070(arg0 + 0xB0, 0x30);
        if (h == 0xFFFFFF) {
            break;
        }
        *(float *)0x70003A20 = func_001CD2B0(8.0f, 16.0f, 384.0f, 128.0f);
        if (*(float *)0x70003A20 == 0.0f) {
            break;
        }
        j = 0;
        do {
            func_001029C0(D_700036A0);
            func_00102948(D_700036D0,
                          *(char **)(src + (*(int *)tbl * 4) + 0x110) + 0xC0);
            func_001CFAE0(sp60, 0, D_700036A0,
                          *(float *)(tbl + 0x40), *(float *)(tbl + 0x20),
                          *(float *)0x70003A20, 9.999999974752427e-07f);
            func_001CFBE0(h, 0, D_00255320, sp60, 0);
            ph = *(float *)(tbl + 0x40) + 0.2f;
            *(float *)(tbl + 0x40) = ph;
            if (ph > 1.4f) {
                *(float *)(tbl + 0x40) = *(float *)(tbl + 0x40) - 1.4f;
                *(int *)tbl = func_00122BB8() % *(unsigned char *)(src + 0xC);
            }
            j++;
            tbl += 4;
        } while (j < 8);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
