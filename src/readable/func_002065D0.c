// NEARMISS func_002065D0  (vram 0x002065D0, 0x19C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 92.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Load order and register allocation of the output-ring arithmetic (expression orders measured;
// body follows the original).
//
// The function links from the asm body in src/func_002065D0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Movie audio: moves decoded audio from the source ring into the output ring
// in 1 KB units. r[0] is the source mode: 0 and 3 do nothing (return 0);
// 1 is a linear source (one span at base +0x44 + pos +0x54 modulo size +0x48,
// size - pos bytes); 2 asks func_00206770 for the readable spans with the
// producer position func_0011A938(0). The output ring (+0x30 base, +0x34
// write position, +0x38 free bytes, +0x3C size) offers its free space,
// rounded down to 1 KB, as up to two spans. When both sides hold at least
// 1 KB, func_00206810 copies (returns the byte count); the output free count
// drops by it, the source position +0x54 and the wrapped read position +0x4C
// advance. Returns the bytes copied.
typedef struct AudioRing {
    int mode;           /* 0x00 */
    char pad04[0x2C];
    char *out_base;     /* 0x30 */
    int out_wpos;       /* 0x34 */
    int out_free;       /* 0x38 */
    int out_size;       /* 0x3C */
    char pad40[4];
    char *base;         /* 0x44 */
    int size;           /* 0x48 */
    int rpos;           /* 0x4C */
    char pad50[4];
    int pos;            /* 0x54 */
} AudioRing;

extern int func_0011A938(int a);
extern void func_00206770(char **ptr1, int *len1, char **ptr2, int *len2, AudioRing *r, int write);
extern int func_00206810(char *s1, int sn1, char *s2, int sn2, char *d1, int n1, char *d2, int n2);

int func_002065D0(AudioRing *r) {
    char *p1;
    char *p2;
    int n1;
    int n2;
    int done = 0;
    int wpos;
    int avail;
    char *d1;
    int dn1;
    int dn2;

    switch (r->mode) {
    case 0:
        return 0;
    case 1:
        p1 = r->base + r->pos % r->size;
        n1 = r->size - r->pos;
        p2 = 0;
        n2 = 0;
        break;
    case 2:
        func_00206770(&p1, &n1, &p2, &n2, r, func_0011A938(0));
        break;
    case 3:
        return 0;
    }
    wpos = (r->out_wpos - r->out_free + r->out_size) % r->out_size;
    avail = r->out_free >> 10 << 10;
    d1 = r->out_base + wpos;
    dn1 = r->out_base + r->out_size - d1;
    if (avail < dn1) {
        dn1 = avail;
    }
    dn2 = avail - dn1;
    if (n1 + n2 >= 0x400 && dn1 + dn2 >= 0x400) {
        done = func_00206810(p1, n1, p2, n2, d1, dn1, r->out_base, dn2);
    }
    r->out_free -= done;
    r->pos += done;
    r->rpos = (r->rpos + done) % r->size;
    return done;
}
