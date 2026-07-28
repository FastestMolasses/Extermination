// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Surface-probe/classify helper. Seeds the scratchpad probe-parameter block at
// 0x70003600 (two zero words, 10.0f = 0x41200000, zero word), builds a probe
// vector at D_70003610 from arg0's position block (+0xD0) via
// func_001026A0/func_001028B8 (arg1 = the transform/offset operand), then runs
// the query func_0019AFE0(arg0, arg1, D_70003610, 6). On a miss returns 2.
// On a hit, caches the hit-attribute byte ((*(char **)0x700031D0)[0x1A], the
// global hit-record's surface id) into arg0+0x23B and classifies it against
// the surface id expected for the requested check `arg2`:
//   arg2 == 0 -> 0 if the byte is 0x32, else 1
//   arg2 == 1 -> 0 if the byte is 0x3B, else 1
//   arg2 == 2 -> 0 if the byte is 0x33, else 1 (also 1 for arg2 > 2)
// (0 = expected surface hit, 1 = hit something else, 2 = no hit.)
//
// Byte-exact only with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 = 91.67%
// (991202 refuses the beqz-slot return-value fill and the early-return jump
// threading 2.3.3 reproduces). Shape notes: multi-exit is load-bearing —
// arm 0 uses bare `return 0;/return 1;`, arm 1 must `return r;` (reusing the
// r=1 materialized before the arg2==1 test), and the miss path is the final
// `return 2;` (mwcc fills the beqz delay slot with the li v0,2).

extern void func_001026A0(int *dst, char *src, int *params);
extern void func_001028B8(int *dst, int *src, int arg2);
extern int func_0019AFE0(char *arg0, int arg1, int *arg2, int arg3);
extern int D_70003600[];
extern int D_70003610[];

int func_00180300(char *arg0, int arg1, int arg2) {
    int r;
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x41200000;
    *(int *)0x7000360C = 0;
    func_001026A0(D_70003610, arg0 + 0xD0, D_70003600);
    func_001028B8(D_70003610, D_70003610, arg1);
    if (func_0019AFE0(arg0, arg1, D_70003610, 6) != 0) {
        *(unsigned char *)(arg0 + 0x23B) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        if (arg2 == 0) {
            if (*(unsigned char *)(arg0 + 0x23B) == 0x32) {
                return 0;
            }
            return 1;
        }
        r = 1;
        if (arg2 == 1) {
            if (*(unsigned char *)(arg0 + 0x23B) == 0x3B) {
                return 0;
            }
            return r;
        }
        if (arg2 == 2) {
            if (*(unsigned char *)(arg0 + 0x23B) == 0x33) {
                r = 0;
            }
        }
        return r;
    }
    return 2;
}
