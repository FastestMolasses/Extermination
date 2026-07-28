// NEARMISS func_001A2370  (vram 0x001A2370, 0x768 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 register-coloring + list-scheduler permutation. Structure/logic fully recovered and verified (frame 0xC0, cnt/idx correctly spilled to sp+0xA0/0xB0, all callee-saved roles for the mesh case snapped in: n=s0, q2=s1, n2=s2, p=s3, vp=s4, k=s5, mtx=s6, vout=s7, hull=s8 after decl-order tuning...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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

extern void func_001026A0(void *in, void *mtx, void *out);
extern float func_00102738(void *a, void *b);

void func_001A2370(unsigned char *actor, float *mtx)
{
    float *c;
    int n;
    float *q2;
    int n2;
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
    float *dst;
    float *src;
    float *mn;
    int i;

    uid = (*(unsigned short *)(actor + 0xE) >> 8) & 0xFF;
    if (uid == 0xFF) {
        return;
    }
    tbl = *(unsigned char **)0x70003250;
    off = *(int *)(tbl + uid * 4 + 4);
    if (off == 0) {
        return;
    }
    if (uid < *(short *)0x7000324C) {
        hp = (unsigned char *)((int)tbl + 0x18) + off;
        cnt = *(short *)hp;
        hull = tbl + off;
        if (*(unsigned short *)(hp + 4) & 0x800) {
            p = hp + 4;

            if (cnt != 0) {
                float *q = D_70003400;

                for (i = 0; i < 3; i++) {
                    q[0] = 3.4e38f;
                    q[4] = -3.4e38f;
                    q++;
                }
            }

            for (idx = 0; idx < cnt; idx++) {
                switch (*(unsigned short *)p & 0xF000) {
                case 0x8000:
                    {
                        c = (float *)(p + 4);
                        src = (float *)(p + 0x14);
                        dst = D_70003600;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }
                        *(float *)0x7000360C = 1.0f;
                        func_001026A0(D_70003600, mtx, D_70003600);
                        src = D_70003600;
                        dst = c;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }

                        src = c;
                        dst = D_70003600;
                        mn = D_70003400;
                        for (i = 0; i < 3; i++) {
                            *dst = *src - c[3];
                            if (!(*mn <= *dst)) {
                                *mn = *dst;
                            }
                            src++;
                            dst++;
                            mn++;
                        }

                        src = c;
                        dst = D_70003600;
                        mn = D_70003400;
                        for (i = 0; i < 3; i++) {
                            *dst = c[3] + *src;
                            if (mn[4] < *dst) {
                                mn[4] = *dst;
                            }
                            src++;
                            dst++;
                            mn++;
                        }
                        p += 0x24;
                    }
                    break;
                case 0x4000:
                    {
                        c = (float *)(p + 4);
                        src = (float *)(p + 0x18);
                        dst = D_70003600;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }
                        *(float *)0x7000360C = 1.0f;
                        func_001026A0(D_70003600, mtx, D_70003600);
                        src = D_70003600;
                        dst = c;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }

                        *(float *)0x70003680 = c[0] - c[3];
                        if (!(*(float *)0x70003400 <= *(float *)0x70003680)) {
                            *(float *)0x70003400 = *(float *)0x70003680;
                        }
                        *(float *)0x70003680 = c[2] - c[3];
                        if (!(*(float *)0x70003408 <= *(float *)0x70003680)) {
                            *(float *)0x70003408 = *(float *)0x70003680;
                        }
                        *(float *)0x70003680 = c[1] - c[4];
                        if (!(*(float *)0x70003404 <= *(float *)0x70003680)) {
                            *(float *)0x70003404 = *(float *)0x70003680;
                        }
                        *(float *)0x70003680 = c[0] + c[3];
                        if (*(float *)0x70003410 < *(float *)0x70003680) {
                            *(float *)0x70003410 = *(float *)0x70003680;
                        }
                        *(float *)0x70003680 = c[2] + c[3];
                        if (*(float *)0x70003418 < *(float *)0x70003680) {
                            *(float *)0x70003418 = *(float *)0x70003680;
                        }
                        *(float *)0x70003680 = c[1] + c[4];
                        if (*(float *)0x70003414 < *(float *)0x70003680) {
                            *(float *)0x70003414 = *(float *)0x70003680;
                        }
                        p += 0x2C;
                    }
                    break;
                case 0x1000:
                    {
                        n = p[2];
                        vout = (float *)(p + 4);
                        vp = (float *)p + (n * 0x18 + 0x14) / 4;
                        src = vp;
                        dst = D_70003600;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }
                        *(float *)0x7000360C = 0.0f;
                        func_001026A0(D_70003600, mtx, D_70003600);
                        src = D_70003600;
                        dst = vout;
                        for (i = 0; i < 3; i++) {
                            *dst = *src;
                            src++;
                            dst++;
                        }

                        q2 = vout;
                        n2 = n * 2;
                        for (k = 0; k < n2; k++) {
                            *(float *)0x70003600 = vp[4];
                            *(float *)0x70003604 = vp[5];
                            *(float *)0x70003608 = vp[6];
                            if (k < n) {
                                *(float *)0x7000360C = 1.0f;
                            } else {
                                *(float *)0x7000360C = 0.0f;
                            }
                            func_001026A0(D_70003600, mtx, D_70003600);
                            q2[4] = *(float *)0x70003600;
                            q2[5] = *(float *)0x70003604;
                            q2[6] = *(float *)0x70003608;
                            if (k < n) {
                                float x = *(float *)0x70003600;
                                float y = *(float *)0x70003604;
                                float z = *(float *)0x70003608;

                                if (!(*(float *)0x70003400 <= x)) {
                                    *(float *)0x70003400 = x;
                                }
                                if (!(*(float *)0x70003404 <= y)) {
                                    *(float *)0x70003404 = y;
                                }
                                if (!(*(float *)0x70003408 <= z)) {
                                    *(float *)0x70003408 = z;
                                }
                                if (*(float *)0x70003410 < x) {
                                    *(float *)0x70003410 = x;
                                }
                                if (*(float *)0x70003414 < y) {
                                    *(float *)0x70003414 = y;
                                }
                                if (*(float *)0x70003418 < z) {
                                    *(float *)0x70003418 = z;
                                }
                            }
                            vp += 3;
                            q2 += 3;
                        }

                        src = vout;
                        dst = D_70003610;
                        {
                            float *dst2 = D_70003620;

                            for (i = 0; i < 3; i++) {
                                *dst = *src;
                                *dst2 = src[4];
                                src++;
                                dst++;
                                dst2++;
                            }
                        }
                        *(float *)0x7000362C = 0.0f;
                        *(float *)0x7000361C = 0.0f;
                        vout[3] = func_00102738(D_70003610, D_70003620);
                        p = (unsigned char *)((Adv48 *)(p + 0x24) + n);
                    }
                    break;
                }
            }

            if (cnt != 0) {
                float *q = D_70003400;

                for (i = 0; i < 3; i++) {
                    *(float *)hull = q[0];
                    *(float *)(hull + 0xC) = q[4];
                    q++;
                    hull += 4;
                }
            }
        }
    }
}
