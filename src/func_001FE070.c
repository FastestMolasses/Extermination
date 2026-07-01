// NEARMISS func_001FE070  (vram 0x001FE070, 0x3F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Logic fully recovered and cross-checked against a sibling caller (func_001FD0E0.c, which calls this exact function and documents its 4-arg void-return convention) plus sibling record-dispatch func_001FDDB0.c (same record-tag 0/2/3/4 shapes on a structurally identical interpreter). Verified: the f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00121A28(unsigned char *p, int v, int n);
extern int func_001232E0(unsigned char *p);
extern int func_001CC170(unsigned char *p);
extern void func_001FC770(int a0, int a1, unsigned char *a2, void *a3);
extern int func_001FE460(int *a0);
extern unsigned char *func_001FE480(int *a0, int a1);
extern int func_001FE4B0(int *a0, int a1);
extern int *func_001FE4D0(int a0, int a1, unsigned int a2);

extern void *D_00264CD0;
extern int D_00264CD8;
extern int D_00264CE0;
extern signed char *D_00264CE4;
extern int D_0026EC10[];
extern int D_00275C50;
extern unsigned char D_00275C55;
extern unsigned char D_00820F90[0x80];

int func_001FE070(int *arg0, int arg1, int arg2, int arg3) {
    int nrec;
    int nbuf;
    unsigned char *buf;
    int *rec;
    int i;
    int lit;
    int obuf;
    int pen;
    int line;
    int rc;
    int *r;
    unsigned char c;
    unsigned char tmp[2];

    line = arg2;
    pen = arg3;
    lit = 0;

    nbuf = func_001FE460(arg0);
    if (arg1 < 0 || arg1 >= nbuf) {
        return 0;
    }

    nrec = func_001FE4B0(arg0, arg1);
    buf = func_001FE480(arg0, arg1);
    rec = func_001FE4D0((int)arg0, arg1, 0);
    obuf = func_001232E0(buf);

    if (nrec == 0) {
        func_001FC770(arg2, pen, buf, &D_00264CD0);
        return 1;
    }

    func_00121A28(D_00820F90, 0, 0x80);
    i = 0;
    nbuf = 0;
    rc = 0;
    while (rc < nrec) {
        r = (int *)((char *)rec + rc * 0x10);
        if (i == r[2]) {
            func_001FC770(line, pen, D_00820F90, &D_00264CD0);
            switch (r[0]) {
            case 0:
                break;
            case 2:
                *(int *)D_00264CE4 = D_0026EC10[r[1]];
                break;
            case 3:
                *(signed char *)(D_00264CE4 + 5) = (signed char)(r[1] * 8);
                break;
            case 4:
                D_00275C50 = D_0026EC10[r[1]];
                D_00275C55 = *((unsigned char *)r + 0xC) * 8;
                break;
            }
            rc += 1;
            line = arg2 + lit;
            nbuf = 0;
            func_00121A28(D_00820F90, 0, 0x80);
            r = (int *)((char *)rec + rc * 0x10);
            while (rc < nrec) {
                if (r[-2] != r[2]) {
                    break;
                }
                r = (int *)((char *)r + 0x10);
                rc += 1;
            }
        }
        c = buf[i];
        if (c == 0xA) {
            func_001FC770(line, pen, D_00820F90, &D_00264CD0);
            line = arg2;
            lit = 0;
            nbuf = 0;
            pen += (D_00264CD8 + D_00264CE0) >> 1;
            func_00121A28(D_00820F90, 0, 0x80);
        } else {
            tmp[0] = c;
            tmp[1] = 0;
            lit += func_001CC170(tmp);
            D_00820F90[nbuf] = c;
            nbuf += 1;
        }
        i += 1;
    }

    if (i < obuf) {
        do {
            c = buf[i];
            if (c == 0xA) {
                func_001FC770(line, pen, D_00820F90, &D_00264CD0);
                line = arg2;
                nbuf = 0;
                pen += (D_00264CD8 + D_00264CE0) >> 1;
                func_00121A28(D_00820F90, 0, 0x80);
            } else {
                tmp[0] = c;
                tmp[1] = 0;
                lit += func_001CC170(tmp);
                D_00820F90[nbuf] = c;
                nbuf += 1;
            }
            i += 1;
        } while (i < obuf);
    }

    func_001FC770(line, pen, D_00820F90, &D_00264CD0);
    return 1;
}
