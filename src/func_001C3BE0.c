// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// VU0/scratchpad (0x70003000 region) matrix setup. Zeroes/initialises the
// working matrix at 0x70003000 (func_001029C0), copies the two direction
// vectors out of arg1 (+0x70..+0x78 and +0x80..+0x88) into the matrix rows
// at 0x70003020/0x70003010 and into the two w=1.0f vec4 operands at
// 0x70003610/0x70003620, builds an orientation from them via func_00102718
// into 0x70003600, copies that result vec3 down to the matrix row at
// 0x70003000, then builds a second matrix at 0x70003440 from the two scalar
// params arg0+0xC0 / arg0+0xC4 (func_00102B08 / func_00102BB0) and composes
// it into 0x70003000 (func_001026D0).
//
// The scratchpad accesses are absolute ($at-form) volatile stores, matching
// the target's per-access `lui $at, 0x7000`; only the pointer arguments go
// through the relocated D_70003xxx symbols.
//
// The three temporaries of the 0x3600 -> 0x3000 vec3 copy are declared in
// REVERSE use order on purpose: mwcc assigns float locals to $f0,$f1,$f2 in
// declaration order, and the target colours them $f2,$f1,$f0, so declaring
// c,b,a and using a,b,c reproduces the target's register numbering exactly.

extern void func_001029C0(void *);
extern void func_00102718(void *, void *, void *);
extern void func_00102B08(void *, void *, float);
extern void func_00102BB0(void *, void *, float);
extern void func_001026D0(void *, void *, void *);

extern unsigned char D_70003000;
extern unsigned char D_70003440;
extern unsigned char D_70003600;
extern unsigned char D_70003610;
extern unsigned char D_70003620;

void func_001C3BE0(char *arg0, char *arg1) {
    func_001029C0(&D_70003000);
    *(volatile float *)0x70003020 = *(float *)(arg1 + 0x70);
    *(volatile float *)0x70003024 = *(float *)(arg1 + 0x74);
    *(volatile float *)0x70003028 = *(float *)(arg1 + 0x78);
    *(volatile float *)0x70003010 = *(float *)(arg1 + 0x80);
    *(volatile float *)0x70003014 = *(float *)(arg1 + 0x84);
    *(volatile float *)0x70003018 = *(float *)(arg1 + 0x88);
    *(volatile float *)0x70003610 = *(float *)(arg1 + 0x80);
    *(volatile float *)0x70003614 = *(float *)(arg1 + 0x84);
    *(volatile float *)0x70003618 = *(float *)(arg1 + 0x88);
    *(volatile int *)0x7000361C = 0x3F800000;
    *(volatile float *)0x70003620 = *(float *)(arg1 + 0x70);
    *(volatile float *)0x70003624 = *(float *)(arg1 + 0x74);
    *(volatile float *)0x70003628 = *(float *)(arg1 + 0x78);
    *(volatile int *)0x7000362C = 0x3F800000;
    func_00102718(&D_70003600, &D_70003610, &D_70003620);
    {
        float c, b, a;
        a = *(volatile float *)0x70003600;
        b = *(volatile float *)0x70003604;
        c = *(volatile float *)0x70003608;
        *(volatile float *)0x70003000 = a;
        *(volatile float *)0x70003004 = b;
        *(volatile float *)0x70003008 = c;
    }
    func_001029C0(&D_70003440);
    func_00102B08(&D_70003440, &D_70003440, *(float *)(arg0 + 0xC0));
    func_00102BB0(&D_70003440, &D_70003440, *(float *)(arg0 + 0xC4));
    func_001026D0(&D_70003000, &D_70003000, &D_70003440);
}
