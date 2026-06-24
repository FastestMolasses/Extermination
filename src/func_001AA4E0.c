// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// "Find nearest qualifying object" scan. Bails returning 0 if scratchpad byte
// 0x70003B8D is set. Otherwise walks the list of D_00275B94 object pointers at
// D_00275B8C (both gp-relative small-data), keeping the object with the
// smallest scratch metric (float 0x70003A20). An object qualifies when its
// (+2 & 0x1F) == 2, its (+0x34) short is nonzero, its type byte (+3) is one of
// {1,2,4,5,6,7,0xC,8}, and func_001AA2A0(arg0, p, func_001AA410(p)) is nonzero.
// Returns the best object pointer (NULL if none).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// only 91.86% (loop saved-reg / scheduling divergence). The loop counter must
// be a plain int (a short local triggers spurious dsll32/dsra32 re-sign-
// extension); the type test is a C switch to reproduce CW's beql compare-chain
// with cases in reverse-chain source order; saved-reg allocation (s0=p,
// s1=result, s2=pp) won by declaration order p; result; pp. objdiff 100.0 vs
// build/expected/func_001AA4E0.o.
extern int func_001AA2A0(int, char *, float);
extern float func_001AA410(char *);
extern char **D_00275B8C;
extern short D_00275B94;

char *func_001AA4E0(int arg0) {
    float best;
    int n;
    char *p;
    char *result;
    char **pp;
    unsigned char t;
    float v;

    best = 1000.0f;
    result = 0;
    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    n = D_00275B94;
    pp = D_00275B8C;
    if (n != 0) {
        do {
            p = *pp;
            n -= 1;
            pp += 1;
            if (((*(unsigned char *)(p + 2) & 0x1F) == 2) && (*(short *)(p + 0x34) != 0)) {
                t = *(unsigned char *)(p + 3);
                switch (t) {
                case 1:
                case 2:
                case 4:
                case 5:
                case 6:
                case 7:
                case 0xC:
                case 8:
                    if (func_001AA2A0(arg0, p, func_001AA410(p)) != 0) {
                        v = *(float *)0x70003A20;
                        if (v < best) {
                            best = v;
                            result = p;
                        }
                    }
                    break;
                }
            }
        } while (n != 0);
    }
    return result;
}
