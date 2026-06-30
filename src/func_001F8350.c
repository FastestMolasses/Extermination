// NEARMISS func_001F8350  (vram 0x001F8350, 0x484 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.70% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation in the particle loop: the four loop induction values (LCG rng / matrix ptr / counter / particle handle) map to a different permutation of {s0,s1,s3,s4} than the target, cascading to the div/add.s operand coloring and the top-dispatch state-byte register (a1 vs a0) ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 92.7%, 991202: 81.8%). Logic fully recovered. State machine on
// the entity byte p[4]: state 0 inits a 64-float 'age' array D_0081F970 to -1.0
// and seeds an LCG (D_0081FA70 holds 64 quaternion-ish 0x10 slots, w-component
// at D_0081FA7C). states 1/2 advance: if the linked obj (p[0x24]) is dying
// (obj[4]==3) propagate state 3 and exit; otherwise every 8th frame
// (0x70003B68 & 7) it picks one of three source matrices via
// func_00122BB8()%3, copies it into the next ring slot, resets that slot's age,
// and advances the ring index (wrap at 0x40). It then builds a scratch matrix
// (0x700038A0 block, scale 0x20/0x70/0x80) via func_001F4A00 and runs the
// 64-entry particle loop: per live slot it computes a pseudo-random spawn value
// (LCG high word / 65535 + 1e-4), and when a duration probe (func_001CD2B0) is
// nonzero emits a particle (func_001CFAE0/func_001CFBE0), ageing each slot by
// 0.015 up to 1.2 then retiring it (-1). state 3 -> func_001AFC10.
//
// WALL: register-allocation permutation in the particle loop. The four loop
// induction values (LCG rng, matrix ptr, counter, particle handle) get assigned
// to a different permutation of {s0,s1,s3,s4} than the target (e.g. rng->s0 in
// target vs s1 here), which also flips the `div`/`add.s` operand coloring and
// the top-dispatch byte register (a1 vs a0) + a branch-likely sense. Not the
// clean-store nop, not jr-table/VU0 — permuter territory.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern void func_001029C0(void *a, int v);
extern int func_00122BB8(int seed);
extern void func_001AFC10(int state);
extern int func_001CD070(void *a, int n);
extern float func_001CD2B0(float a, float b, float c, float d);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFBE0(int handle, int n, void *p, void *src);
extern void func_001F4A00(void *a, void *b);

extern float D_0081F970[64];
extern char D_0081FA70[];
extern char D_0081FA7C[];
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_0025D890[];

void func_001F8350(char *p) {
    char sp60[0x60];
    char *obj;
    int *q;
    float *fp;
    char *mat;
    int i;
    int idx;
    int sel;
    int rng;
    int st;
    float t;

    obj = *(char **)(p + 0x24);
    q = (int *)(p + 0x1F0);
    st = *(unsigned char *)(p + 4);
    switch (st) {
    case 0:
        fp = D_0081F970;
        i = 0;
        do {
            i += 1;
            *fp = -1.0f;
            fp += 1;
        } while (i < 0x40);
        q[0] = 0;
        q[1] = func_00122BB8(i);
        *(unsigned char *)(p + 4) = 1;
        /* fallthrough */
    case 1:
    case 2:
        if (*(unsigned char *)(obj + 4) == 3) {
            *(unsigned char *)(p + 4) = 3;
            return;
        }
        if (*(unsigned char *)(p + 4) == 1) {
            if ((*(int *)0x70003B68 & 7) == 0) {
                sel = func_00122BB8(*(unsigned char *)(obj + 4)) % 3;
                idx = q[0];
                switch (sel) {
                case 0:
                    *(float *)0x700038A0 = 3.2f;
                    *(float *)0x700038A4 = -1.6f;
                    *(float *)0x700038A8 = -0.6f;
                    *(float *)0x700038AC = 1.0f;
                    func_001026A0(D_0081FA70 + idx * 0x10, *(char **)(obj + 0x118) + 0x90, D_700038A0);
                    break;
                case 1:
                    func_00102948(D_0081FA70 + idx * 0x10, *(char **)(obj + 0x134) + 0xC0);
                    break;
                case 2:
                    func_00102948(D_0081FA70 + idx * 0x10, *(char **)(obj + 0x130) + 0xC0);
                    break;
                }
                idx = q[0];
                *(float *)(D_0081FA7C + idx * 0x10) = 1.0f;
                D_0081F970[q[0]] = 0.0f;
                q[0] = q[0] + 1;
                if (q[0] >= 0x40) {
                    q[0] = 0;
                }
            }
        }
        *(float *)0x700038A0 = 3.2f;
        *(float *)0x700038A4 = -1.5f;
        *(float *)0x700038A8 = -0.6f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, *(char **)(obj + 0x118) + 0x90, D_700038A0);
        *(int *)0x700038B0 = 0x20;
        *(int *)0x700038B4 = 0x70;
        *(int *)0x700038B8 = 0x80;
        *(int *)0x700038BC = 0x80;
        func_001F4A00(D_700038A0, D_700038B0);
        fp = D_0081F970;
        rng = q[1];
        mat = D_0081FA70;
        i = 0;
        do {
            *(float *)0x70003A20 = (float)((rng >> 16) & 0xFFFF) / 65535.0f + 0.0001f;
            rng = rng * 0x25 + 0xB;
            if (!(*fp < 0.0f)) {
                func_001029C0(D_700036A0, 0x477FFF00);
                func_00102918(D_700036A0, D_700036A0, mat);
                sel = func_001CD070(D_700036D0, 0x30);
                *(float *)0x70003A24 = func_001CD2B0(6.0f, 6.0f, 96.0f, 96.0f);
                t = *(float *)0x70003A24;
                if (t != 0.0f) {
                    func_001CFAE0(sp60, 0, D_700036A0, *fp, *(float *)0x70003A20, t, 1.0e-6f);
                    func_001CFBE0(sel, 1, D_0025D890, sp60);
                }
                *fp = *fp + 0.015f;
                if (!(*fp < 1.2f)) {
                    *fp = -1.0f;
                }
            }
            i += 1;
            fp += 1;
            mat += 0x10;
        } while (i < 0x40);
        return;
    case 3:
        func_001AFC10(st);
        return;
    }
}
