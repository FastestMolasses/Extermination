// NEARMISS func_0021AE90  (vram 0x0021AE90, 0x2F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Best build is mwcc233 (91.60%) vs mwcc991202 (87.39%). Full logic recovered and confirmed against docs/FINDINGS.md's func_001EFEB0/func_00128640 siblings: a debris/tendril particle-spawner state machine (state 0 seeds a 16-entry model/color table via func_00122BB8() RNG; state 1 processes up to 2...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Debris/tendril-object particle spawner state machine, dispatched on
// arg0+4 (state byte): state 0 seeds a 16-entry (model,color) table via
// func_00122BB8() % 6 lookups into D_00266AE0/D_00266B00, resets the
// walk cursor (+0x80/+0x84) and advances to state 1; state 1 processes
// up to 2 table entries per call: transforms the bone-space endpoint
// (arg1[entry.model]+0xC0) and (arg1[entry.color]+0xC0) via
// func_00102948/func_001028D0, computes the segment length via the
// FPU-MAC adda.s/madd.s sum-of-squares + func_0011E748 (sqrtf) idiom,
// scales it by 1.1 and stores it at the VU0 scratch D_70003A20, derives
// a spawn direction (func_00102760/func_001CD390/func_00102918) and
// spawns a debris FX object (func_001EFEB0(0x8000003B, dir)), writing
// its lifetime fields (+0x1F0=6, +0x1F4=segment length, +0x1F8=0.2f);
// after 16 entries walked, advances arg0+4 to state 3; states 2/3 call
// the cleanup func_001AFC10(arg0) and return.
extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_001CD390(void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern float func_0011E748(float d);
extern int func_00122BB8(void);
extern char *func_001EFEB0(int model, void *pos);
extern void func_001AFC10(char *p);

extern int D_00266AE0[];
extern int D_00266B00[];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

char func_0021AE90(char *arg0, int arg1) {
    char *s2;
    char *s1;
    char *s3;
    int i;
    int entry_model;
    int entry_color;
    char *v0;

    s1 = *(char **)(arg0 + 0x24);
    s2 = arg0 + 0x1F0;

    switch (*(unsigned char *)(arg0 + 4)) {
    default:
        if (*(unsigned char *)(arg0 + 4) != 0) {
            return 0;
        }
        s3 = s2;
        for (i = 0; i < 0x10; i++, s3 += 8) {
            *(int *)s3 = D_00266AE0[(func_00122BB8() >> 0x10) % 6];
            *(int *)(s3 + 4) = D_00266B00[(func_00122BB8() >> 0x10) % 6];
        }
        *(int *)(s2 + 0x84) = 0;
        *(int *)(s2 + 0x80) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        if (*(unsigned char *)(s1 + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            return 0;
        }
        if (*(int *)(s2 + 0x80) != 0x10) {
            i = 0;
            switch (*(int *)(s2 + 0x80)) {
            case 0:
            case 4:
            case 8:
                i = 0;
                /* fallthrough */
                do {
                case 12:
                    v0 = s2 + *(int *)(s2 + 0x84) * 8;
                    entry_model = *(int *)v0;
                    entry_color = *(int *)(v0 + 4);
                    func_00102948(D_700038A0, (void *)(*(int *)(s1 + entry_model * 4 + 0x110) + 0xC0));
                    func_00102948(D_700038B0, (void *)(*(int *)(s1 + entry_color * 4 + 0x110) + 0xC0));
                    func_001028D0(D_700038B0, D_700038B0, D_700038A0);
                    *(float *)0x70003A20 =
                        func_0011E748(*(float *)0x700038B4 * *(float *)0x700038B4 +
                                      *(float *)0x700038B0 * *(float *)0x700038B0 +
                                      *(float *)0x700038B8 * *(float *)0x700038B8);
                    *(float *)0x70003A20 = *(float *)0x70003A20 * 1.1f;
                    func_00102760(D_700038B0, D_700038B0);
                    func_001CD390(D_700036A0, D_700038B0);
                    func_00102918(D_700036A0, D_700036A0, D_700038A0);
                    v0 = func_001EFEB0(0x8000003B, D_700036A0);
                    if (v0 != 0) {
                        *(char *)(v0 + 5) = 1;
                        *(int *)(v0 + 0x1F0) = 6;
                        *(float *)(v0 + 0x1F4) = *(float *)0x70003A20;
                        *(int *)(v0 + 0x1F8) = 0x3E4CCCCD;
                    }
                    i += 1;
                    *(int *)(s2 + 0x84) = *(int *)(s2 + 0x84) + 1;
                } while (i < 2);
                break;
            }
        } else {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        *(int *)(s2 + 0x80) = *(int *)(s2 + 0x80) + 1;
        return 0;
    case 3:
    case 2:
        func_001AFC10(arg0);
        return 0;
    }
}
