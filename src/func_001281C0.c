// NEARMISS func_001281C0  (vram 0x001281C0, 0x8C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.29% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc codegen function (not mwcc): boot uses sd/ld $ra, xori-equality, sltiu, movn. Body 100% correct at 78.3% with ee-gcc 2.9-991111-01 -O2. Residual = ee-gcc xori `==const` lowering (boot xori;beqz vs our li;beq) + convert-block register coloring / branch-likely sense. Both mwcc builds <28%. P...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Float-to-fixed/int converter. Classifies the float arg via func_001278C0
// (fills a 4-word Cls: w0=class, w1=sign, w2=exponent, w3=mantissa). class 2
// (and class 0/1) -> 0; class 4 or exponent>=0x1F -> signed saturate
// (sign?0x80000000:0x7FFFFFFF); exponent<0 -> 0; otherwise the mantissa is
// right-shifted by (0x1E - exponent) and negated when the sign word is set.
//
// This SDK math routine is ee-gcc codegen (sibling func_001280B8 is // COMPILER:
// eegcc -O2): the boot build saves $ra as a doubleword (sd/ld, not mwcc's
// sq/lq), tests equality with xori, and uses sltiu/movn -- all ee-gcc shapes.
// NEARMISS 78.3% with ee-gcc 2.9-991111-01 -O2; both mwcc builds are far worse
// (mwcc 991202 27.7%, mwcc 2.3.3 12.0%), confirming it is NOT an mwcc function.
// Body/control-flow fully recovered; the residual is the ee-gcc `xori reg,k`
// equality-compare lowering (boot uses xori;beqz, our build emits li;beq) plus
// a register-coloring/branch-likely-sense difference in the convert block.
// permuter / ee-gcc-codegen wall.
typedef struct { int w[4]; } Cls;
extern void func_001278C0(float *in, Cls *out);

int func_001281C0(float x) {
    Cls s;
    float fx;
    int cls;
    int exp;
    unsigned int m;

    fx = x;
    func_001278C0(&fx, &s);
    cls = s.w[0];
    if (cls == 2) return 0;
    if ((unsigned int)cls < 2) return 0;
    if (cls != 4) {
        exp = s.w[2];
        if (exp < 0) return 0;
        if (exp < 0x1F) goto conv;
        exp = 0x1E;
    }
    if (s.w[1]) return (int)0x80000000;
    return 0x7FFFFFFF;
conv:
    m = (unsigned int)s.w[3] >> (0x1E - exp);
    if (s.w[1]) return -(int)m;
    return (int)m;
}
