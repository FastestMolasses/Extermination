// NEARMISS func_001B8AB0  (vram 0x001B8AB0, 0x504 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 84.38%): every case ends in break and the function has one shared
// return 0 at the end (the original's single zero-return exit); the state-1 RNG arms each write
// their own 0x70003608/0x7000360C pair; the state-2 offsets call the RNG inside the expression;
// the blend source is the relocated symbol D_70003600; the timer step is a compound += 1.0f.
// Remaining differences: (1) prologue: the original keeps the switch value in $a2 (copying arg2
// to $s0 first) where this compile uses $a0; local copies and declaration orders of arg2 did not
// move it. (2) state 2: the original loads arg2+0x14 once before each RNG call and again after
// it for the halving term; this compile reuses the first load for both. (3) one empty delay
// slot in the state-1 parameter test.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// A 3-way switch on arg2+8 (particle-pair state 0/1/2), each case
// sharing an identical inner state machine on arg1+4 (0=init copy of two
// D_008105D0/D_008105E0 static vectors into the object at arg2+0x20/+0x30 and
// zero the arg2+0x10 timer, falling through to 1=advance the timer by 1.0
// until it reaches arg2+0xC, at which point the two vectors are copied BACK
// to the statics and the routine returns 1). On every odd tick it derives a
// small random offset (case-specific formula) into the VU0 scratch block at
// 0x70003600/0x70003610 and blends it into both object vectors via
// func_001026A0 (build) + func_001028B8 (apply).
extern void func_00102948(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int float_to_int(float);
extern int func_00122BB8(void);

extern float D_008105D0;
extern float D_008105E0;
extern float D_00810650;
extern float D_70003610;
extern float D_70003600;

int func_001B8AB0(void *arg0, char *arg1, char *p) {
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
            *(float *)(p + 0x10) += 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                *(volatile float *)0x70003600 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                *(volatile int *)0x70003608 = 0;
                *(volatile int *)0x7000360C = 0x3F800000;
                func_001026A0(&D_70003610, &D_00810650, &D_70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            break;
        }
        break;
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
            *(float *)(p + 0x10) += 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                if (*(int *)(p + 0x14) == 0) {
                    *(volatile int *)0x70003600 = 0;
                    *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 7) - 4) / 10.0f;
                    *(volatile int *)0x70003608 = 0;
                    *(volatile int *)0x7000360C = 0x3F800000;
                } else {
                    *(volatile int *)0x70003600 = 0;
                    *(volatile float *)0x70003604 = (float) ((func_00122BB8() & 0xF) - 8) / 10.0f;
                    *(volatile int *)0x70003608 = 0;
                    *(volatile int *)0x7000360C = 0x3F800000;
                }
                func_001026A0(&D_70003610, &D_00810650, &D_70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            break;
        }
        break;
    case 2:
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
            *(float *)(p + 0x10) += 1.0f;
            if (float_to_int(*(float *)(p + 0x10)) & 1) {
                *(volatile float *)0x70003600 = (float) ((((func_00122BB8() >> 0x10) * *(int *)(p + 0x14)) >> 0xF) - (*(int *)(p + 0x14) >> 1)) / 10.0f;
                *(volatile float *)0x70003604 = (float) ((((func_00122BB8() >> 0x10) * *(int *)(p + 0x14)) >> 0xF) - (*(int *)(p + 0x14) >> 1)) / 10.0f;
                *(volatile int *)0x70003608 = 0;
                *(volatile int *)0x7000360C = 0x3F800000;
                func_001026A0(&D_70003610, &D_00810650, &D_70003600);
                func_001028B8(&D_008105D0, p + 0x20, &D_70003610);
                func_001028B8(&D_008105E0, p + 0x30, &D_70003610);
            }
            break;
        }
        break;
    }
    return 0;
}
