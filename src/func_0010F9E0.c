// NEARMISS func_0010F9E0  (vram 0x0010F9E0, 0xF0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.15% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc codegen-shape wall (confirmed). C is semantically correct and reaches 57.15% best at -O2 (-O1=49%, -O0=0%), but cannot byte-match because expected .text is 288 bytes (72 instrs) vs our 232 bytes (58 instrs). The original SDK object was built by a compiler whose optimizer differs structurally from our ee-gcc 2....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241C90;
extern unsigned char D_00279340[];
extern int D_00279240;
extern int D_00279280;

extern int func_0010E8A8(int *a, int b, int c, unsigned char *d, int e,
                         int *f, int g, int h, int i);

int func_0010F9E0(unsigned char *src, int a1)
{
    int i;
    int n;
    int c;

    if (D_00241C90 < 0)
        return 0;

    c = src[0];
    D_00279340[4] = c;
    i = 0;
    if (c != 0) {
        i = 1;
        for (;;) {
            if (i >= 0xFC)
                break;
            c = src[i];
            D_00279340[4 + i] = c;
            if (c == 0)
                break;
            i++;
        }
    }

    n = i;
    if (i != 0xFC) {
        *(int *)D_00279340 = a1;
        D_00279340[0xFF] = 0;
        *(int *)D_00279340 = a1;
        n = 0xFB;
    }
    n += 5;

    D_00279340[0xFF] = 0;
    if (func_0010E8A8(&D_00279240, 3, 0, D_00279340, n, &D_00279280, 4, 0, 0) >= 0)
        return D_00279280;

    return -1;
}
