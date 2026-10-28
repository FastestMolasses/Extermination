// NEARMISS func_001A2370  (vram 0x001A2370, 0x768 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.49% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was 80.63%; matching wave 2026-10-27).
// Fixes: every scratchpad word is a relocated extern (// SPAD:), the vector copies use block-local walkers, the
// mesh point min/max reads the transformed lanes from the scratch words directly, and the lane loop bound is n * 2.
// Remaining differences: the original fills three branch delay slots from the fall-through block (the off == 0
// return, the uid-count guard and the first case compare), which mwcc 2.3.3 never does here (2.3-991202 does, but
// differs elsewhere); plus saved-register colouring of the lane loop (bound temp, p, vp, k) and the copy-loop counters.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000324C 0x70003250 0x70003400 0x70003404 0x70003408 0x70003410 0x70003414 0x70003418 0x70003600 0x70003604 0x70003608 0x7000360C 0x7000361C 0x7000362C 0x70003680

// SEMANTICS: re-transform an actor-attached collision hull by a matrix and
// rebuild its AABB. Looks up the actor's uid hull (0x70003250 table, uid from
// actor +0xE high byte, 0xFF = none; offset word at +4, gated uid <
// 0x7000324C count) and requires the extended (0x800) prim format on the
// first prim. Seeds the scratch min/max at D_70003400 (min xyz) /
// D_70003410 (max xyz) with +/-3.4e38, then walks the prim list at
// hull+0x18 (count at +0, prims at +4) dispatching on the 0xF000 type
// nibble:
//   0x8000 (box, 0x24 bytes): transform center (+0x14 -> +4 via the
//     scratch vector D_70003600 and func_001026A0 with w=1), then min/max
//     the +/-radius (+0xC) box into D_70003400/D_70003410 (3-lane loops).
//   0x4000 (capsule, 0x2C bytes): transform center (+0x18 -> +4, w=1),
//     then six unrolled min/max updates staged through the scratch float
//     0x70003680: x/z use radius +0xC, y uses half-height +0x10.
//   0x1000 (mesh, 0x24 + n*0x30 bytes, n = byte +2): transform the axis
//     vector (float row at (n*0x18+0x14)/4 words, w=0) into +4, then per
//     lane (2n lanes: first n = points w=1, rest = edge vectors w=0)
//     transform +0x10..0x18 in place; point lanes also min/max into
//     D_70003400/D_70003410. Finally func_00102738(D_70003610,
//     D_70003620) (axis dot origin with w lanes zeroed) -> +0xC.
// Afterwards writes the six accumulated bounds back to the hull header
// (min xyz at +0, max xyz at +0xC).

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte mesh lane record */

extern float D_70003400[];   /* scratch AABB: min xyz, +0x10 max xyz */
extern float D_70003600[];   /* scratch transform vector (w at +0xC) */
extern float D_70003610[];
extern float D_70003620[];

extern short D_7000324C;
extern unsigned char *D_70003250;
extern float D_70003404;
extern float D_70003408;
extern float D_70003410;
extern float D_70003414;
extern float D_70003418;
extern float D_70003604;
extern float D_70003608;
extern float D_7000360C;
extern float D_7000361C;
extern float D_7000362C;
extern float D_70003680;
extern void func_001026A0(void *in, void *mtx, void *out);
extern float func_00102738(void *a, void *b);

void func_001A2370(unsigned char *actor, float *mtx)
{
    float *c;
    int n;
    float *q2;
    unsigned char *p;
    float *vp;
    int k;
    float *vout;
    unsigned char *hull;
    int cnt;
    int idx;
    int uid;
    int off;
    unsigned char *tbl;
    unsigned char *hp;

    uid = (*(unsigned short *)(actor + 0xE) >> 8) & 0xFF;
    if (uid == 0xFF) {
        return;
    }
    tbl = D_70003250;
    off = *(int *)(tbl + uid * 4 + 4);
    if (off == 0) {
        return;
    }
    if (uid < D_7000324C) {
        hp = (unsigned char *)((int)tbl + 0x18) + off;
        cnt = *(short *)hp;
        hull = tbl + off;
        if (*(unsigned short *)(hp + 4) & 0x800) {
            p = hp + 4;

            if (cnt != 0) {
                float *q = D_70003400;
                int j;

                for (j = 0; j < 3; j++) {
                    q[0] = 3.4e38f;
                    q[4] = -3.4e38f;
                    q++;
                }
            }

            for (idx = 0; idx < cnt; idx++) {
                switch (*(unsigned short *)p & 0xF000) {
                case 0x8000:
                    c = (float *)(p + 4);
                    {
                        float *s = (float *)(p + 0x14);
                        float *d = D_70003600;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = *s;
                            s++;
                            d++;
                        }
                    }
                    D_7000360C = 1.0f;
                    func_001026A0(D_70003600, mtx, D_70003600);
                    {
                        float *s = D_70003600;
                        float *d = c;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = *s;
                            s++;
                            d++;
                        }
                    }
                    {
                        float *s = c;
                        float *d = D_70003600;
                        float *m = D_70003400;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = *s - c[3];
                            if (!(*m <= *d)) {
                                *m = *d;
                            }
                            s++;
                            d++;
                            m++;
                        }
                    }
                    {
                        float *s = c;
                        float *d = D_70003600;
                        float *m = D_70003400;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = c[3] + *s;
                            if (m[4] < *d) {
                                m[4] = *d;
                            }
                            s++;
                            d++;
                            m++;
                        }
                    }
                    p += 0x24;
                    break;
                case 0x4000:
                    c = (float *)(p + 4);
                    {
                        float *s = (float *)(p + 0x18);
                        float *d = D_70003600;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = *s;
                            s++;
                            d++;
                        }
                    }
                    D_7000360C = 1.0f;
                    func_001026A0(D_70003600, mtx, D_70003600);
                    {
                        float *s = D_70003600;
                        float *d = c;
                        int j;

                        for (j = 0; j < 3; j++) {
                            *d = *s;
                            s++;
                            d++;
                        }
                    }

                        D_70003680 = c[0] - c[3];
                        if (!(D_70003400[0] <= D_70003680)) {
                            D_70003400[0] = D_70003680;
                        }
                        D_70003680 = c[2] - c[3];
                        if (!(D_70003408 <= D_70003680)) {
                            D_70003408 = D_70003680;
                        }
                        D_70003680 = c[1] - c[4];
                        if (!(D_70003404 <= D_70003680)) {
                            D_70003404 = D_70003680;
                        }
                        D_70003680 = c[0] + c[3];
                        if (D_70003410 < D_70003680) {
                            D_70003410 = D_70003680;
                        }
                        D_70003680 = c[2] + c[3];
                        if (D_70003418 < D_70003680) {
                            D_70003418 = D_70003680;
                        }
                        D_70003680 = c[1] + c[4];
                        if (D_70003414 < D_70003680) {
                            D_70003414 = D_70003680;
                        }
                        p += 0x2C;
                    break;
                case 0x1000:
                        n = p[2];
                        vout = (float *)(p + 4);
                        vp = (float *)p + (n * 0x18 + 0x14) / 4;
                        {
                            float *s = vp;
                            float *d = D_70003600;
                            int j;

                            for (j = 0; j < 3; j++) {
                                *d = *s;
                                s++;
                                d++;
                            }
                        }
                        D_7000360C = 0.0f;
                        func_001026A0(D_70003600, mtx, D_70003600);
                        {
                            float *s = D_70003600;
                            float *d = vout;
                            int j;

                            for (j = 0; j < 3; j++) {
                                *d = *s;
                                s++;
                                d++;
                            }
                        }

                        q2 = vout;
                        for (k = 0; k < n * 2; k++) {
                            D_70003600[0] = vp[4];
                            D_70003604 = vp[5];
                            D_70003608 = vp[6];
                            if (k < n) {
                                D_7000360C = 1.0f;
                            } else {
                                D_7000360C = 0.0f;
                            }
                            func_001026A0(D_70003600, mtx, D_70003600);
                            q2[4] = D_70003600[0];
                            q2[5] = D_70003604;
                            q2[6] = D_70003608;
                            if (k < n) {
                                if (!(D_70003400[0] <= D_70003600[0])) {
                                    D_70003400[0] = D_70003600[0];
                                }
                                if (!(D_70003404 <= D_70003604)) {
                                    D_70003404 = D_70003604;
                                }
                                if (!(D_70003408 <= D_70003608)) {
                                    D_70003408 = D_70003608;
                                }
                                if (D_70003410 < D_70003600[0]) {
                                    D_70003410 = D_70003600[0];
                                }
                                if (D_70003414 < D_70003604) {
                                    D_70003414 = D_70003604;
                                }
                                if (D_70003418 < D_70003608) {
                                    D_70003418 = D_70003608;
                                }
                            }
                            vp += 3;
                            q2 += 3;
                        }

                        {
                            float *s = vout;
                            float *d = D_70003610;
                            float *d2 = D_70003620;
                            int j;

                            for (j = 0; j < 3; j++) {
                                *d = *s;
                                *d2 = s[4];
                                s++;
                                d++;
                                d2++;
                            }
                        }
                        D_7000362C = 0.0f;
                        D_7000361C = 0.0f;
                        vout[3] = func_00102738(D_70003610, D_70003620);
                        p = (unsigned char *)((Adv48 *)(p + 0x24) + n);
                    break;
                }
            }

            if (cnt != 0) {
                float *q = D_70003400;
                int j;

                for (j = 0; j < 3; j++) {
                    *(float *)hull = q[0];
                    *(float *)(hull + 0xC) = q[4];
                    q++;
                    hull += 4;
                }
            }
        }
    }
}
