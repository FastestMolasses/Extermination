// NEARMISS func_00110508  (vram 0x00110508, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.28% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler + loop-rotation tail-duplication + GPR-coloring wall. Body logic fully recovered (strlen-of-arg, length+0xB < 0x51 gate calling func_00122B58(D_0026B9C0,s) on the >=0x51 branch, else func_0010E088(0)/func_0010E248() then copy D_0026B9B0 string + the arg string into a 0x50 stack buffer, NUL-termi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_0026B9B0[];
extern char D_0026B9C0[];
extern void func_00122B58(char *fmt, char *s);
extern void func_0010E088(int);
extern void func_0010E248(void);
extern void func_00110368(char *buf, int x);

int func_00110508(char *s) {
    char buf[0x50];
    char *base = &D_0026B9B0[0];
    char *p;
    char *src;
    unsigned char *dst;

    p = s;
    if (*p != 0) {
        do {
            p++;
        } while (*p != 0);
        if ((unsigned int)(p - (s - 0xB)) >= 0x51) {
            func_00122B58(D_0026B9C0, s);
            return 0;
        }
    } else {
        if ((unsigned int)(p - (s - 0xB)) >= 0x51) {
            func_00122B58(D_0026B9C0, s);
            return 0;
        }
    }

    func_0010E088(0);
    func_0010E248();

    src = base;
    dst = (unsigned char *)buf;
    while (*src != 0) {
        *dst = *src;
        dst++;
        src++;
    }
    src = s;
    while (*src != 0) {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = 0;
    func_00110368(buf, 0);
    return 0;
}
