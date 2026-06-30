// NEARMISS func_001FCF90  (vram 0x001FCF90, 0x148 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// saved-register coloring permutation: q/loop-counter/count/buffer-pointer occupy a permuted s0/s1/s4/s5 assignment vs the target (target q=s0,i=s1,n=s4,vs5=s5; mwcc gives vs5=s0,q=s1,i=s4,n=s5), plus one commutative addu operand order (addu s1,v0,v1 vs s1,v1,v0). Frame size (0x2F0), control flow, ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 93.1% (mwcc233). Setup loop over an entity resolved from D_0028A49C
// (base = p + *(int*)p, then + *(int*)(p+0x30)); func_001FE460 primes it and
// func_001FE480 looks up entry q for arg0. If q is live (*q != 0), func_001FE660
// gives an upper count n; the loop runs up to 10 passes while (arg1*0xA + i) < n,
// staging each pass through func_001FE530 into a 0x280 stack buffer (0x40 stride)
// and emitting via func_001FC770(0x38, label, ..., D_00264CF0) with label starting
// at 0x2F and advancing +0xC after the first pass. Returns 1.
// Body / frame / control-flow byte-identical to target; sole residual is a
// saved-register coloring permutation (s0/s1/s4/s5 roles permuted) — backend.
extern void func_001FE460(char *p);
extern unsigned char *func_001FE480(char *p, int idx);
extern unsigned char *func_001FE530(char *buf, unsigned char *p, int n);
extern int func_001FE660(unsigned char *p);
extern void func_001FC770(int a, int b, char *buf, char *tbl);
extern char *D_0028A49C;
extern char D_00264CF0[];

int func_001FCF90(int arg0, int arg1) {
    char buf[0x280];
    char *vs5;
    char *p;
    unsigned char *q;
    int base;
    int label;
    int i;
    int n;

    base = arg1 * 0xA;
    label = 0x2F;
    p = D_0028A49C;
    p = (char *)((p + *(int *)p) + *(int *)(p + 0x30));
    func_001FE460(p);
    q = func_001FE480(p, arg0);
    if (*q != 0) {
        n = func_001FE660(q);
        vs5 = buf;
        i = 0;
        while (i < 0xA && base + i < n) {
            if (i != 0) {
                label += 0xC;
                q = func_001FE530(vs5, q, 0);
                func_001FC770(0x38, label, vs5, D_00264CF0);
            } else {
                q = func_001FE530(buf, q, base);
                func_001FC770(0x38, label, buf, D_00264CF0);
            }
            vs5 += 0x40;
            i++;
        }
    }
    return 1;
}
