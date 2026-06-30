// NEARMISS func_00113D08  (vram 0x00113D08, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.39% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR coloring wall (no permuter lever, confirmed s84). Full CFG + instruction stream + 0xA0 frame all match via goto-loop structure; sole residual is register coloring: the r>>16 / r&0xFFFF value pair and the two loop-cached global base pointers (D_00241CF8, D_00241D50) landed in swapped saved regs ($s0<->$s1, ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241CF8;
extern int D_00241D40;
extern unsigned char D_00241D50[];
extern char D_0026BDC8[];
extern char D_0026BDF8[];
extern char D_0026BE40[];

extern int func_00122B58(char *fmt, ...);
extern void func_0010DFD8(int a, int b);
extern unsigned int func_00113F68(int a0, int a1, int a2, int a3, unsigned char *t0);

int func_00113D08(int a0, int a1, int a2, int *a3) {
    int s2 = a0;
    int s4 = a1;
    int s0 = a2;
    int *fp = a3;
    int s1;
    int s3;
    int s6;
    unsigned int r;

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BDC8, s2);
    }

    s6 = 0;
    if (D_00241D40 == 0) {
        return 0;
    }

    s3 = 0;
    func_0010DFD8(s4, s2 << 11);

    if (s0 != 0) {
        goto body;
        while (1) {
            if (s0 != 0 && s1 == 0) goto done;
        body:
            r = func_00113F68(0, s2 - s3, s4 + (s3 << 11), 2, D_00241D50);
            s1 = r & 0xFFFF;
            s0 = r >> 16;
            s3 = s3 + s1;
            if (s0 != 0 && D_00241CF8 > 0) {
                s6 = s0;
                func_00122B58(D_0026BDF8, s3, s1, s2, s0);
            }
            if (s3 == s2) break;
        }
    done:
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BE40);
        }
        *fp = s6;
        return s3;
    }

    r = func_00113F68(0, s2, s4, 2, D_00241D50);
    s0 = r >> 16;
    s3 = r & 0xFFFF;
    *fp = s0;
    return s3;
}
