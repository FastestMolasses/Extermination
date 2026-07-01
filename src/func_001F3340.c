// NEARMISS func_001F3340  (vram 0x001F3340, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Switch-dispatch head (15 case labels, non-monotonic target test order) fully recovered by writing case-groups in REVERSE of their target test order (discovered: mwcc tests switch case-groups in reverse source-declaration order). Residual is a genuine mwcc list-scheduling interleave in the 4-float...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *dst, void *src);
extern void func_001031E0(void *a, void *b);
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;

void func_001F3340(char *arg0, char *arg1, int arg2) {
    int div;
    int step;
    int i;

    switch (arg2) {
    case 0:
    case 1:
    case 4:
    case 5:
    case 6:
        step = 0xC;
        goto body;
    case 2:
    case 10:
    case 23:
        step = 6;
        goto body;
    case 3:
    case 7:
    case 8:
    case 9:
        step = 4;
        goto body;
    case 17:
    case 18:
    case 22:
        *(int *)(arg0 + 0x74) = 0xB4;
        break;
    default:
        *(int *)(arg0 + 0x74) = 1;
        break;
    }
    *(int *)(arg0 + 0x78) = 1;
    return;
body:
    div = 0x78 / step;
    *(int *)(arg0 + 0x74) = 0;
    func_00102948(&D_700038A0, arg0);
    func_00102948(&D_700038B0, arg0 + 0x10);
    i = 0;
    if (div > 0) {
        do {
            float t;
            float b4;

            b4 = *(float *)0x700038B4;
            t = (float) step;
            b4 = b4 + *(float *)(arg1 + 0x48) * t;
            *(float *)0x700038B4 = b4;
            *(float *)0x700038C0 = *(float *)0x700038A0 + *(float *)0x700038B0 * t;
            *(float *)0x700038C4 = *(float *)0x700038A4 + b4 * t;
            *(float *)0x700038C8 = *(float *)0x700038A8 + *(float *)0x700038B8 * t;
            if (func_0019A570(&D_700038A0, &D_700038C0, 4, 0) != 0) {
                char *p;
                func_00102948(arg0 + 0x60, &D_700031B0);
                p = *(char **)0x700031D0;
                *(float *)(arg0 + 0x50) = *(float *)(p + 0x24);
                *(float *)(arg0 + 0x54) = *(float *)(p + 0x28);
                *(float *)(arg0 + 0x58) = *(float *)(p + 0x2C);
                goto after_loop;
            }
            func_001031E0(&D_700038A0, &D_700038C0);
            i += 1;
            *(int *)(arg0 + 0x74) = *(int *)(arg0 + 0x74) + step;
        } while (i < div);
    }
after_loop:
    if (i == div) {
        *(int *)(arg0 + 0x7C) = 1;
        *(int *)(arg0 + 0x74) = step * div;
        *(int *)(arg0 + 0x78) = 1;
        return;
    }
    *(int *)(arg0 + 0x7C) = 0;
    *(int *)(arg0 + 0x74) = *(int *)(arg0 + 0x74) + step;
    if (*(int *)(arg0 + 0x74) <= 0) {
        *(int *)(arg0 + 0x74) = 1;
    }
    *(int *)(arg0 + 0x78) = 1;
}
