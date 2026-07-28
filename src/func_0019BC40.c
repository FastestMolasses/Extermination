// NEARMISS func_0019BC40  (vram 0x0019BC40, 0xAB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.42% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 web-coloring-order permutation + list-sched ties (sibling family wall of func_0019F730/func_001A1390, which parked at 88.78%/76%). Body and structure are fully register-shape-correct except: (1) the two loop-1 record blocks emit the same 5 instrs with mtc1 zero,$f0 two slots later than ta...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: melee/lock-on target scan around point pos (float[3]).
//   Pass 1: walks the published CLASS-4 actor list (D_00275B7C/D_00275B84); for each actor
//   with a valid uid (byte at +0xE hi) looks up its area hull in the uid table at 0x70003250,
//   AABB-tests pos.x/pos.z, then walks the hull prim list (+0x18 count, +0x1C prims)
//   dispatching on the 0xF000 type nibble (0x8000 advance-only, 0x4000 func_001A56A0,
//   0x2000 func_001A5760, 0x1000 func_001A58B0 with 0x30/0x18-stride tri records). Each hit
//   can record up to two candidates (out[0]/out[2] pair flagged 0x8000 with spad 0x7000319C
//   aux, and out[1]/out[3] pair with 0x700031AC aux), capped at 20.
//   Then func_0019F1A0(pos, 0x33) stages the grid, and blocks A-D pick the shortest row span
//   from the 4 span tables (0x70003228/2C/38/3C staged through the scratchpad mailbox shorts
//   0x70003B86/B88), choosing table id 0/1/4/5 into which/start/end.
//   Pass 2: walks D_70003210[which][start..end): 64-byte grid entries, gated by the span
//   shorts 0x70003240/42/48/4A, |entry.f28| >= 0.001f, entry.b1A < 0x50 and
//   func_0019F330(v1, v2, q, ent); hits record dist=q[1], aux=q[3], flag 0x4000.
//   Candidates are selection-sorted ascending by dist via the order[] index array; close
//   pairs (|d1-d2| < 3.0) get flag 0x80 (culled) when hard-flagged 0x8000 or actor hp
//   (+0x1A) < 0x32; far pairs clear 0x80. Endpoint fixups on order[0]/order[n-1] by flag
//   bit0. Survivors are compacted into the result tables D_70003170 (flags),
//   D_700030F0 (dist), D_70003130 (object), D_00282250 (aux); count -> 0x700031E0.

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte tri record */
typedef struct Adv24 { int w[6]; } Adv24;    /* 0x18-byte tri record */

extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern float D_700030F0[];
extern unsigned char *D_70003130[];
extern unsigned short D_70003170[];
extern short *D_70003210[];
extern float D_00282250[];

extern float func_0011DF78(float);
extern void func_0019F1A0(float *, int);
extern int func_0019F330(float *, float *, float *, void *);
extern int func_001A56A0(float *, float *, void *);
extern int func_001A5760(float *, float *, void *);
extern int func_001A58B0(float *, float *, void *);

void func_0019BC40(float *pos)
{
    int end;
    int which;
    int start;
    float v1[4];
    float v2[4];
    float q[4];
    float out[4];
    short order[20];
    short flags[20];
    float dist[20];
    unsigned char *obj[20];
    float extra[20];
    int i;
    int j;
    int n;
    int k;
    int best;
    int cnt;
    int f;
    int b;
    unsigned char **list;
    short *sp2;
    unsigned char *ent;

    for (i = 0; i < 20; i++) {
        order[i] = -1;
    }
    n = 0;
    for (i = 0; i < 3; i++) {
        float tv = pos[i];
        v2[i] = tv;
        v1[i] = tv;
    }
    v2[1] += 1.0f;

    list = D_00275B7C;
    for (i = 0; i < D_00275B84; i++) {
        unsigned char *a = *list++;
        if (a[0] == 0) {
            continue;
        }
        if ((a[2] & 0x1F) != 4) {
            continue;
        }
        {
            int uid = *(unsigned short *)(a + 0xE) >> 8;

            if (uid == 0xFF) {
                continue;
            }
            {
                unsigned char *tbl = *(unsigned char **)0x70003250;
                int off = *(int *)(tbl + uid * 4 + 4);

                if (off == 0) {
                    continue;
                }
                {
                    unsigned char *hull = tbl + off;

                    if (pos[0] < *(float *)(hull + 0)) {
                        continue;
                    }
                    if (!(pos[0] <= *(float *)(hull + 0xC))) {
                        continue;
                    }
                    if (pos[2] < *(float *)(hull + 8)) {
                        continue;
                    }
                    if (!(pos[2] <= *(float *)(hull + 0x14))) {
                        continue;
                    }
                    q[0] = pos[0];
                    q[1] = pos[2];
                    {
                        short *np = (short *)(hull + 0x18);
                        unsigned char *p = (unsigned char *)np + 4;
                        int hit;

                        for (j = 0; j < *np; j++) {
                            short w = *(short *)p;

                            switch (w & 0xF000) {
                            case 0x8000:
                                hit = 0;
                                if (w & 0x800) {
                                    p += 0x24;
                                } else {
                                    p += 0x14;
                                }
                                break;
                            case 0x4000:
                                hit = func_001A56A0(q, out, p);
                                if (*(short *)p & 0x800) {
                                    p += 0x2C;
                                } else {
                                    p += 0x18;
                                }
                                break;
                            case 0x2000:
                                hit = func_001A5760(q, out, p);
                                p += 0x1C;
                                break;
                            case 0x1000:
                                hit = func_001A58B0(q, out, p);
                                if (*(short *)p & 0x800) {
                                    int m = p[2];
                                    Adv48 *t48 = (Adv48 *)(p + 0x24);
                                    p = (unsigned char *)(t48 + m);
                                } else {
                                    int m = p[2];
                                    Adv24 *t24 = (Adv24 *)(p + 0x14);
                                    p = (unsigned char *)(t24 + m);
                                }
                                break;
                            }
                            if (hit == 0) {
                                continue;
                            }
                            if (!(n < 20)) {
                                break;
                            }
                            if (out[0] > -3.4e37f) {
                                order[n] = n;
                                dist[n] = out[0];
                                obj[n] = a;
                                flags[n] = 0x8000;
                                extra[n] = *(float *)0x7000319C;
                                if (!(out[2] <= 0.0f)) {
                                    flags[n] |= 1;
                                }
                                n++;
                            }
                            if (!(n < 20)) {
                                break;
                            }
                            if (out[1] < 3.4e37f) {
                                order[n] = n;
                                dist[n] = out[1];
                                obj[n] = a;
                                flags[n] = 0x8000;
                                extra[n] = *(float *)0x700031AC;
                                if (!(out[3] <= 0.0f)) {
                                    flags[n] |= 1;
                                }
                                n++;
                            }
                        }
                    }
                }
            }
        }
    }

    func_0019F1A0(pos, 0x33);

    best = *(int *)0x7000320C;
    {
        int t, e, s;
        short tmp = *(short *)0x70003240;
        short *tab = *(short **)0x70003228;
        *(volatile short *)0x70003B88 = tmp;
        t = *(volatile short *)0x70003B88;
        *(volatile short *)0x70003B86 = tab[t];
        *(volatile short *)0x70003B88 = t + 1;
        e = *(volatile short *)0x70003B88;
        s = *(volatile short *)0x70003B86;
        if (e - s < best) {
            best = e - s;
            start = s;
            end = e;
            which = 0;
        }
    }
    {
        int t, e;
        short tmp = *(short *)0x70003242;
        short *tab = *(short **)0x7000322C;
        *(volatile short *)0x70003B86 = tmp;
        t = *(volatile short *)0x70003B86;
        *(volatile short *)0x70003B88 = tab[t];
        e = *(volatile short *)0x70003B88;
        if (e - t < best) {
            end = e;
            best = e - t;
            start = t;
            which = 1;
        }
    }
    {
        int t, e, s;
        short tmp = *(short *)0x70003248;
        short *tab = *(short **)0x70003238;
        *(volatile short *)0x70003B88 = tmp;
        t = *(volatile short *)0x70003B88;
        *(volatile short *)0x70003B86 = tab[t];
        *(volatile short *)0x70003B88 = t + 1;
        e = *(volatile short *)0x70003B88;
        s = *(volatile short *)0x70003B86;
        if (e - s < best) {
            start = s;
            best = e - s;
            end = e;
            which = 4;
        }
    }
    {
        int t, e;
        short tmp = *(short *)0x7000324A;
        short *tab = *(short **)0x7000323C;
        *(volatile short *)0x70003B86 = tmp;
        t = *(volatile short *)0x70003B86;
        *(volatile short *)0x70003B88 = tab[t];
        e = *(volatile short *)0x70003B88;
        if (e - t < best) {
            start = t;
            end = e;
            which = 5;
        }
    }

    sp2 = D_70003210[which] + start;
    for (; start < end; start++) {
        ent = *(unsigned char **)0x70003208 + (*sp2++ << 6);
        if (*(short *)0x70003240 < *(short *)(ent + 0xC)) {
            continue;
        }
        if (*(short *)(ent + 0xE) < *(short *)0x70003242) {
            continue;
        }
        if (func_0011DF78(*(float *)(ent + 0x28)) < 0.001f) {
            continue;
        }
        if (*(short *)(ent + 0x14) > *(short *)0x70003248) {
            continue;
        }
        if (*(short *)(ent + 0x16) < *(short *)0x7000324A) {
            continue;
        }
        if (*(unsigned char *)(ent + 0x1A) >= 0x50) {
            continue;
        }
        if (func_0019F330(v1, v2, q, ent) == 0) {
            continue;
        }
        if (!(n < 20)) {
            break;
        }
        dist[n] = q[1];
        obj[n] = ent;
        order[n] = n;
        flags[n] = 0x4000;
        if (!(q[3] <= 0.0f)) {
            flags[n] |= 1;
        }
        extra[n] = q[3];
        n++;
    }

    for (i = 0; i < n; i++) {
        k = i;
        for (j = i + 1; j < n; j++) {
            if (!(dist[order[k]] <= dist[order[j]])) {
                k = j;
            }
        }
        if (k != i) {
            short tmp = order[i];
            order[i] = order[k];
            order[k] = tmp;
        }
    }

    if (n <= 0) {
        *(int *)0x700031E0 = 0;
        return;
    }

    {
        for (i = 0; i < n - 1; i++) {
            if (func_0011DF78(dist[order[i]] - dist[order[i + 1]]) < 3.0f) {
                b = order[i + 1];
                f = flags[b];
                if (f & 0x8000) {
                    flags[b] = f | 0x80;
                } else if (obj[b][0x1A] < 0x32) {
                    flags[b] = f | 0x80;
                }
                b = order[i];
                f = flags[b];
                if (f & 0x8000) {
                    flags[b] = f | 0x80;
                } else if (obj[b][0x1A] < 0x32) {
                    flags[b] = f | 0x80;
                }
            } else {
                flags[order[i]] &= ~0x80;
                flags[order[i + 1]] &= ~0x80;
            }
        }
    }

    f = flags[order[0]];
    if (!(f & 1)) {
        flags[order[0]] = f & ~0x80;
    }
    f = flags[order[n - 1]];
    i = 0;
    if (f & 1) {
        flags[order[n - 1]] = f & ~0x80;
        i = 0;
    }

    cnt = 0;
    for (; i < n; i++) {
        if (!(flags[order[i]] & 0x80)) {
            D_70003170[cnt] = flags[order[i]];
            D_700030F0[cnt] = dist[order[i]];
            D_70003130[cnt] = obj[order[i]];
            D_00282250[cnt] = extra[order[i]];
            cnt++;
        }
    }
    *(int *)0x700031E0 = cnt;
}
