// NEARMISS func_001A9480  (vram 0x001A9480, 0x268 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.47% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: dispatch on arg1+3==0xB (special-item path scans a 12-entry table D_0024A840 of indices into arg1+0x110, computing a squared-distance test with +6.0f margin per candidate) vs the default single squared-distance test path; shared tail sets arg1+0x36=0x400A, calls fu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00183C40(char *p, void *m);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern void func_00102948(char *dst, char *src);
extern unsigned char D_0024A840[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001A9480(char *arg0, char *arg1) {
    int hit;
    float v;
    float sq;
    int i;
    unsigned char *idx;
    char *rec;

    if (*(unsigned char *)(arg1 + 3) == 0xB) {
        hit = 0;
        func_00183C40(arg1, &D_700038B0);
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0;
        *(float *)0x70003A20 = func_00102738(&D_700038A0, &D_700038A0);
        v = **(float **)(arg0 + 0x30) + **(float **)(arg1 + 0x30);
        sq = v * v;
        *(float *)0x70003A24 = v;
        *(float *)0x70003A24 = sq;
        if (*(float *)0x70003A20 <= sq) {
            hit = 1;
        } else {
            idx = D_0024A840;
            for (i = 0; i < 0xC; i++, idx++) {
                rec = *(char **)(arg1 + 0x110 + (*idx << 2));
                func_001028D0(&D_700038A0, rec + 0xC0, arg0 + 0xB0);
                *(int *)0x700038AC = 0;
                *(float *)0x70003A20 = func_00102738(&D_700038A0, &D_700038A0);
                v = 6.0f + **(float **)(arg0 + 0x30);
                sq = v * v;
                *(float *)0x70003A24 = v;
                *(float *)0x70003A24 = sq;
                if (*(float *)0x70003A20 <= sq) {
                    hit = 1;
                    break;
                }
            }
        }
        if (hit == 0) {
            return;
        }
    } else {
        func_00183C40(arg1, &D_700038B0);
        func_001028D0(&D_700038A0, &D_700038B0, arg0 + 0xB0);
        *(int *)0x700038AC = 0;
        *(float *)0x70003A20 = func_00102738(&D_700038A0, &D_700038A0);
        v = **(float **)(arg0 + 0x30) + **(float **)(arg1 + 0x30);
        sq = v * v;
        *(float *)0x70003A24 = v;
        *(float *)0x70003A24 = sq;
        if (*(float *)0x70003A20 > sq) {
            return;
        }
    }
    *(short *)(arg1 + 0x36) = 0x400A;
    func_00102948(arg1 + 0x70, arg0 + 0xC0);
    *(short *)0x70003B88 = 0;
}
