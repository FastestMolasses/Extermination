// NEARMISS func_0022D380  (vram 0x0022D380, 0x94C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring permutation ONLY: on mwcc 2.3.3 the body is instruction-for-instruction identical (all 2.3.3-only artifacts reproduce: b+dead-const if/else diamonds, orphan slti after delay-slot fill, bnezl continue); residual is a whole-function callee-saved GPR + FP coloring permutation (st: ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct {
    unsigned int rgb;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char ofs;
} TextCol;

typedef struct {
    int type;
    char *str;
} CreditRow;

typedef struct {
    unsigned char state;   /* 0x00 */
    char pad1[7];
    float y;               /* 0x08 */
    char pad2[8];
    int cnt;               /* 0x14 */
    int timer;             /* 0x18 */
    float frac;            /* 0x1C */
} CreditState;

extern int float_to_int(float);
extern float func_0011DF78(float);
extern int func_001CC170(char *);
extern void func_001CC1E0(int, int, int, int, int, char *, void *);
extern void func_00207F80(int, int, int, int, int, unsigned int);

extern CreditState D_00822CD0;
extern CreditRow D_00268B40[];

int func_0022D380(void) {
    CreditState *st;
    TextCol colA;
    TextCol colB;
    unsigned char *pB;
    CreditRow *row;
    int dy;
    int i;
    int y;
    int x;
    int x2;
    int ty;
    int v;
    float t;

    st = &D_00822CD0;
    pB = (unsigned char *)&colB;
    colA.rgb = 0x606060;
    colA.r = 0x80;
    colA.g = 0;
    colA.ofs = 0;
    *(unsigned int *)pB = 0x606060;
    colB.r = 0x28;
    colB.g = 0;
    colB.ofs = 0;

    switch (st->state) {
    case 0:
        st->state += 1;
        st->cnt = 0;
        st->y = -(0.5f * (float)st->cnt);
        st->frac = -(0.5f * (float)st->cnt);
        while (st->frac < 0.0f) {
            st->frac += 1.0f;
        }
        break;
    case 1:
        st->y -= 0.5f;
        st->frac -= 0.5f;
        t = st->y;
        st->frac = func_0011DF78(t - (float)float_to_int(t));
        if (st->y < 0.0f) {
            if (st->frac == 0.5f) v = 0; else v = 12;
            colA.ofs = v;
            if (st->frac == 0.5f) v = 0; else v = 12;
            colB.ofs = v;
        } else {
            if (st->frac == 0.5f) v = 12; else v = 0;
            colA.ofs = v;
            if (st->frac == 0.5f) v = 12; else v = 0;
            colB.ofs = v;
        }
        for (row = D_00268B40, dy = 0, i = 0; ; row++, dy += 0x15, i++) {
            y = float_to_int(st->y) + dy;
            ty = row->type;
            if (ty == 4) {
                if (y < 0x70) {
                    st->state += 1;
                    st->timer = 200;
                }
                break;
            }
            if (y < -0x8F) {
                continue;
            }
            if (y >= 0x91) {
                break;
            }
            {
                switch (ty) {
                case 3:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    break;
                case 2:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    break;
                case 1:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    x = -(func_001CC170(row->str) >> 1);
                    x2 = x + func_001CC170(row->str);
                    func_00207F80(1, (x + 0x800) << 4, ((y + 0x80C) << 4) + colA.ofs,
                                  (x2 + 0x802) << 4,
                                  ((y + 0x80D) << 4) + colA.ofs, 0x80808080);
                    break;
                }
            }
        }
        st->cnt += 1;
        break;
    case 2:
        t = st->y;
        st->frac = func_0011DF78(t - (float)float_to_int(t));
        if (st->y < 0.0f) {
            if (st->frac == 0.5f) v = 0; else v = 12;
            colA.ofs = v;
            if (st->frac == 0.5f) v = 0; else v = 12;
            colB.ofs = v;
        } else {
            if (st->frac == 0.5f) v = 12; else v = 0;
            colA.ofs = v;
            if (st->frac == 0.5f) v = 12; else v = 0;
            colB.ofs = v;
        }
        for (row = D_00268B40, dy = 0, i = 0; ; row++, dy += 0x15, i++) {
            ty = row->type;
            if (ty == 4) {
                break;
            }
            y = float_to_int(st->y) + dy;
            if (y < -0x8F) {
                continue;
            }
            if (y >= 0x91) {
                break;
            }
            {
                switch (ty) {
                case 3:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    break;
                case 2:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    break;
                case 1:
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x7FF, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x801, y + 0x800, 10, 20, row->str, pB);
                    func_001CC1E0(1, -(func_001CC170(row->str) >> 1) + 0x800, y + 0x800, 10, 20, row->str, &colA);
                    x = -(func_001CC170(row->str) >> 1);
                    x2 = x + func_001CC170(row->str);
                    func_00207F80(1, (x + 0x800) << 4, ((y + 0x80C) << 4) + colA.ofs,
                                  (x2 + 0x802) << 4,
                                  ((y + 0x80D) << 4) + colA.ofs, 0x80808080);
                    break;
                }
            }
        }
        st->timer -= 1;
        if (st->timer < 0) {
            return 1;
        }
        break;
    }
    return 0;
}
