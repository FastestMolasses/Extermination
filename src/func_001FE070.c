// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.89% NEARMISS).
// Fixes read from the original instructions:
// - -sdatathreshold 4: only D_00275C50/D_00275C55 are reached gp-relative; the
//   font-metrics words D_00264CD8/D_00264CE0 and the pointer D_00264CE4 stay
//   absolute (unsized array externs);
// - the old body was logically wrong: the trailing loop's newline also resets the
//   accumulated width 'lit';
// - the record loop is an unrotated 'for (;;) { if (rc >= nrec) break; ... }'
//   (test at the top), the trailing loop a plain while;
// - the bank-count result has its own local (reusing nbuf recoloured the loop
//   registers); records are typed as 16-byte structs;
// - the newline arm advances the pen y before resetting line/lit/nbuf;
// - command 3 stores through an unsigned char pointer (byte load of rec->val).

// Text-box string renderer. arg0 = text bank, arg1 = string index (0 if out of range of
// func_001FE460's count), arg2/arg3 = pen x/y. Fetches the string's control-record count
// (func_001FE4B0), its byte text (func_001FE480), its 16-byte records {type, val, pos, byte}
// (func_001FE4D0) and its length (func_001232E0). With no records the whole string is drawn
// with func_001FC770 against the font D_00264CD0. Otherwise text accumulates into the
// 0x80-byte line buffer D_00820F90; at a record whose pos equals the current byte index the
// pending text is flushed, the command applied (2 = font color word from palette
// D_0026EC10[val], 3 = glyph scale byte at font+5 = val<<3, 4 = global color D_00275C50 and
// scale D_00275C55 from val/byte), line x moves to arg2 + accumulated width, and any following
// records at the same pos are skipped. A newline flushes, returns x to arg2 and advances y by
// (D_00264CD8 + D_00264CE0) >> 1; other bytes are appended and their width (func_001CC170 on a
// "char + NUL" buffer) accumulated. After the records run out the remaining bytes are laid out
// the same way, then the last line is flushed. Returns 1.

typedef struct Rec {
    int type;
    int val;
    int pos;
    unsigned char b;
} Rec;

extern void func_00121A28(unsigned char *p, int v, int n);
extern int func_001232E0(unsigned char *p);
extern int func_001CC170(unsigned char *p);
extern void func_001FC770(int a0, int a1, unsigned char *a2, void *a3);
extern int func_001FE460(int *a0);
extern unsigned char *func_001FE480(int *a0, int a1);
extern int func_001FE4B0(int *a0, int a1);
extern Rec *func_001FE4D0(int *a0, int a1, int a2);

extern char D_00264CD0[];
extern int D_00264CD8[];
extern int D_00264CE0[];
extern unsigned char *D_00264CE4[];
extern int D_0026EC10[2];
extern int D_00275C50;
extern unsigned char D_00275C55;
extern unsigned char D_00820F90[0x80];

int func_001FE070(int *arg0, int arg1, int arg2, int arg3) {
    int i;
    int nbuf;
    int rc;
    int nrec;
    Rec *r;
    int pen;
    int lit;
    int line;
    unsigned char *buf;
    Rec *rec;
    int obuf;
    int n;
    unsigned char c;
    unsigned char tmp[2];

    line = arg2;
    lit = 0;
    pen = arg3;

    n = func_001FE460(arg0);
    if (arg1 < 0 || arg1 >= n) {
        return 0;
    }

    nrec = func_001FE4B0(arg0, arg1);
    buf = func_001FE480(arg0, arg1);
    rec = func_001FE4D0(arg0, arg1, 0);
    obuf = func_001232E0(buf);

    if (nrec == 0) {
        func_001FC770(arg2, pen, buf, D_00264CD0);
        return 1;
    }

    func_00121A28(D_00820F90, 0, 0x80);
    i = 0;
    nbuf = 0;
    rc = 0;
    for (;;) {
        if (rc >= nrec) break;
        r = &rec[rc];
        if (i == r->pos) {
            func_001FC770(line, pen, D_00820F90, D_00264CD0);
            switch (r->type) {
            case 0:
                break;
            case 2:
                *(int *)D_00264CE4[0] = D_0026EC10[r->val];
                break;
            case 3:
                D_00264CE4[0][5] = (unsigned char)r->val << 3;
                break;
            case 4:
                D_00275C50 = D_0026EC10[r->val];
                D_00275C55 = r->b << 3;
                break;
            }
            rc++;
            line = arg2 + lit;
            nbuf = 0;
            func_00121A28(D_00820F90, 0, 0x80);
            while (rc < nrec && rec[rc - 1].pos == rec[rc].pos) {
                rc++;
            }
        }
        c = buf[i];
        if (c == '\n') {
            func_001FC770(line, pen, D_00820F90, D_00264CD0);
            pen += (D_00264CD8[0] + D_00264CE0[0]) >> 1;
            line = arg2;
            lit = 0;
            nbuf = 0;
            func_00121A28(D_00820F90, 0, 0x80);
        } else {
            tmp[0] = c;
            tmp[1] = 0;
            lit += func_001CC170(tmp);
            D_00820F90[nbuf++] = c;
        }
        i++;
    }

    while (i < obuf) {
        c = buf[i];
        if (c == '\n') {
            func_001FC770(line, pen, D_00820F90, D_00264CD0);
            pen += (D_00264CD8[0] + D_00264CE0[0]) >> 1;
            line = arg2;
            lit = 0;
            nbuf = 0;
            func_00121A28(D_00820F90, 0, 0x80);
        } else {
            tmp[0] = c;
            tmp[1] = 0;
            lit += func_001CC170(tmp);
            D_00820F90[nbuf++] = c;
        }
        i++;
    }

    func_001FC770(line, pen, D_00820F90, D_00264CD0);
    return 1;
}
