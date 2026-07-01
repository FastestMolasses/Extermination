// NEARMISS func_001B8AB0  (vram 0x001B8AB0, 0x504 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Prologue register-allocation permutation (which register holds the switch discriminant/arg2 alias across the field load) plus FP even/odd coloring in the case-2 RNG divide chain -- confirmed not source-controllable after >8 structural iterations (dispatch shape, call-arg direction, inverse-CSE, v...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 84.4% (mwcc 2.3.3; 991202 = 82.7%). Logic and control flow fully
// recovered: a 3-way switch on arg2+8 (particle-pair state 0/1/2), each case
// sharing an identical inner state machine on arg1+4 (0=init copy of two
// D_008105D0/D_008105E0 static vectors into the object at arg2+0x20/+0x30 and
// zero the arg2+0x10 timer, falling through to 1=advance the timer by 1.0
// until it reaches arg2+0xC, at which point the two vectors are copied BACK
// to the statics and the routine returns 1). On every odd tick it derives a
// small random offset (case-specific formula) into the VU0 scratch block at
// 0x70003600/0x70003610 and blends it into both object vectors via
// func_001026A0 (build) + func_001028B8 (apply).
//
// WALL: register-allocation permutation in the prologue -- the target keeps
// the incoming arg2 in $a2 across the field load (paddub s0,a2,zero BEFORE
// lw a2,8(a2), clobbering a2 in place), while mwcc allocates a fresh $a0 for
// the switch discriminant and moves arg2 to s0 afterward. This reg-alloc
// choice is not controllable from source (confirmed via case/-> pointer-alias
// experiments). A secondary residual is FP even/odd-half register coloring
// in the case-2 do-twice RNG->float divide chain (target reloads arg2+0x14
// into a fresh register per use; mwcc keeps it live across both jal calls).
// Both are permuter-class scheduling/regalloc artifacts, not logic bugs.
extern void func_00102948(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int float_to_int(float);
extern int func_00122BB8(void);

extern float D_008105D0;
extern float D_008105E0;
extern float D_00810650;
extern float D_70003610;

int func_001B8AB0(void *arg0, char *arg1, char *arg2) {
    char *p = arg2;

    switch (*(int *)(p + 8)) {
    case 0:
        switch (*(unsigned char *)(arg1 + 4)) {
        case 0:
            func_00102948(p + 0x20, &D_008105D0);
            func_00102948(p + 0x30, &D_008105E0);
            *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
            *(float *)(p + 0x10) = 0.0f;
            /* fallthrough */
        case 1:
            if (*(float *)(p + 0x10) >= *(float *)(p + 0xC)) {
                func_00102948(&D_008105D0, p + 0x20);
                func_00102948(&D_008105E0, p + 0x30);
                return 1;
            }
            *(float *)(p + 0x10) = *(float *)(p + 0x10) + 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                *(volatile float *)0x70003600 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                *(volatile int *)0x70003608 = 0;
                *(volatile int *)0x7000360C = 0x3F800000;
                func_001026A0(&D_70003610, &D_00810650, (void *)0x70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            return 0;
        }
        return 0;
    case 1:
        switch (*(unsigned char *)(arg1 + 4)) {
        case 0:
            func_00102948(p + 0x20, &D_008105D0);
            func_00102948(p + 0x30, &D_008105E0);
            *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
            *(float *)(p + 0x10) = 0.0f;
            /* fallthrough */
        case 1:
            if (*(float *)(p + 0x10) >= *(float *)(p + 0xC)) {
                func_00102948(&D_008105D0, p + 0x20);
                func_00102948(&D_008105E0, p + 0x30);
                return 1;
            }
            *(float *)(p + 0x10) = *(float *)(p + 0x10) + 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                if (*(int *)(p + 0x14) == 0) {
                    *(volatile int *)0x70003600 = 0;
                    *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 7) - 4) / 10.0f;
                } else {
                    *(volatile int *)0x70003600 = 0;
                    *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                }
                *(volatile int *)0x70003608 = 0;
                *(volatile int *)0x7000360C = 0x3F800000;
                func_001026A0(&D_70003610, &D_00810650, (void *)0x70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            return 0;
        }
        return 0;
    case 2: {
        int rnd;

        switch (*(unsigned char *)(arg1 + 4)) {
        case 0:
            func_00102948(p + 0x20, &D_008105D0);
            func_00102948(p + 0x30, &D_008105E0);
            *(unsigned char *)(arg1 + 4) = *(unsigned char *)(arg1 + 4) + 1;
            *(float *)(p + 0x10) = 0.0f;
            /* fallthrough */
        case 1:
            if (*(float *)(p + 0x10) >= *(float *)(p + 0xC)) {
                func_00102948(&D_008105D0, p + 0x20);
                func_00102948(&D_008105E0, p + 0x30);
                return 1;
            }
            *(float *)(p + 0x10) = *(float *)(p + 0x10) + 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                rnd = func_00122BB8();
                *(volatile float *)0x70003600 = (float) (((rnd >> 0x10) * *(int *)(p + 0x14) >> 0xF) - (*(int *)(p + 0x14) >> 1)) / 10.0f;
                rnd = func_00122BB8();
                *(volatile float *)0x70003604 = (float) (((rnd >> 0x10) * *(int *)(p + 0x14) >> 0xF) - (*(int *)(p + 0x14) >> 1)) / 10.0f;
                *(volatile int *)0x70003608 = 0;
                *(volatile int *)0x7000360C = 0x3F800000;
                func_001026A0(&D_70003610, &D_00810650, (void *)0x70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            return 0;
        }
        return 0;
    }
    }
    return 0;
}
