// NEARMISS func_001A3980  (vram 0x001A3980, 0x6AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Broad register-allocation/instruction-scheduling permutation across the whole 1708-byte body (s2/s4/s5/s6/s7/s8 register swaps in both passes, dsrl32-vs-dsra32 on the flags tag-mask shift, bne-vs-beql polarity/cascade-order on the 4-way type-nibble dispatch, do-while-guard vs while-loop entry sha...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS (m233_5 decode): segment-cast hit-scan combining the static
// world-cell prim table and the dynamic object table, narrowing the
// segment's hit-interval clamp [lo,hi] as closer hits are found.
//
// Pass 1: for cell in [0, D_7000324C): rec = D_70003250[cell] (an int*4
// per cell: +0 unused here, +4 flags). Skip unless flags&0x80000000 (live)
// and !(flags&0x40000000) (not masked) and (!(flags&0x20000000) ||
// D_7000324E!=0) (extra gate). desc = D_0024D7C0[D_00810700][D_00810701]
// (per-viewer context, a struct-of-arrays selected by two byte indices);
// surf = desc[cell].surfaceByte (+8). Reject unless 0x1E <= surf < 0x5A.
// prim = rec + (sign-extended low 30 bits of flags) -- i.e. the flags word
// doubles as prim-record pointer with the top 2 tag bits masked off.
// Reject unless the query interval brackets prim's AABB (+4/+0x10 Y,
// +0/+0xC X, +8/+0x14 Z against the running lo/hi and the fixed X/Z window
// at D_70003190/98). On acceptance, walk prim's sub-record list (count at
// +0x18, records starting at +0x18+4) dispatching on the type nibble of
// the record's first short: 0x8000 no-op (just skip, stride +0x24 if bit
// 0x800 set else +0x14), 0x4000 -> func_001A44B0 (stride +0x2C/+0x18),
// 0x2000 -> func_001A4650 (fixed stride +0x1C), 0x1000 -> func_001A4030
// (stride +0x24+count*0x30 or +0x14+count*0x18, count = record byte +2).
// Any nonzero callee result is a hit: narrow [lo,hi] to
// [D_700031B4,D_700031B4], stage owner NULL at D_700031D4, OR surf into
// the low byte of D_700030CA, clear ok, and re-split lo/hi around the new
// interval value.
//
// Pass 2: for i in [0, D_00275B84): e = D_00275B7C[i] (pointer list).
// Skip unless e->tag!=0, (e->sub&0x1F)==4, e != D_700031D4-source-object
// (D_70003254). idx = byte 1 of e->f0E (the high byte of the halfword at
// +0xE). Skip unless idx!=0xFF and D_70003250[idx].flags!=0 and
// 0<=idx<D_7000324C. Same AABB test against D_70003250[idx]'s prim
// record (base+flags, no tag-mask this time -- flags itself is the
// prim-record byte offset), same sub-record dispatch loop (unlike pass 1,
// pass 2's 0x8000 case DOES call func_001A44B0, confirmed against the raw
// .s), and on a hit stage owner e at D_700031D4 and OR e->f54 into
// D_700030CA instead.
//
// Returns 1 if pass 1 and pass 2 never recorded a hit, 0 otherwise.
extern int func_001A4030(void *arg0);
extern int func_001A44B0(void *arg0);
extern int func_001A4650(void *arg0);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern char *D_0024D7C0[];

extern unsigned char **D_00275B7C;
extern short D_00275B84;

int func_001A3980(void) {
    float lo, hi;
    float t0, t1;
    int ok;
    int cell;
    char *cellRec;
    int flags;
    char *prim;
    unsigned char surf;
    char *sub;
    int subIdx;
    int subCount;
    int subN;
    int r;
    short kind;
    char *base;
    unsigned char cnt;
    int objCount;
    int obj;
    unsigned char *e;
    int hitIdx;
    int hitFlags;
    char *hitPrim;

    t0 = *(float *)0x70003194;
    t1 = *(float *)0x700031A4;
    ok = 1;
    if (t0 <= t1) {
        lo = t0;
        hi = t1;
    } else {
        lo = t1;
        hi = t0;
    }

    base = *(char **)0x70003250;

    for (cell = 0; cell < *(short *)0x7000324C; cell++) {
        cellRec = base + cell * 4;
        flags = *(int *)(cellRec + 4);
        if (!(flags & 0x80000000)) {
            continue;
        }
        if ((flags & 0x40000000) ||
            ((flags & 0x20000000) && *(short *)0x7000324E == 0)) {
            continue;
        }
        surf = *(unsigned char *)(*(*(char ***)(D_0024D7C0 + D_00810700) + D_00810701) + cell * 0x28 + 8);
        if (surf < 0x1E || surf >= 0x5A) {
            continue;
        }
        prim = base + ((flags << 2) >> 2);
        if (lo < *(float *)(prim + 4) || hi > *(float *)(prim + 0x10)) {
            continue;
        }
        if (*(float *)0x70003190 < *(float *)(prim + 0) || *(float *)0x70003190 > *(float *)(prim + 0xC)) {
            continue;
        }
        if (*(float *)0x70003198 < *(float *)(prim + 8) || *(float *)0x70003198 > *(float *)(prim + 0x14)) {
            continue;
        }

        subCount = *(short *)(prim + 0x18);
        sub = prim + 0x18 + 4;
        r = 0;
        subN = 0;
        while (subN < subCount) {
            kind = *(short *)sub;
            subIdx = kind & 0xF000;
            if (subIdx == 0x8000) {
                if (kind & 0x800) {
                    sub += 0x24;
                } else {
                    sub += 0x14;
                }
            } else if (subIdx == 0x4000) {
                r = func_001A44B0(sub);
                if (*(short *)sub & 0x800) {
                    sub += 0x2C;
                } else {
                    sub += 0x18;
                }
            } else if (subIdx == 0x2000) {
                r = func_001A4650(sub);
                sub += 0x1C;
            } else if (subIdx == 0x1000) {
                r = func_001A4030(sub);
                cnt = *(unsigned char *)(sub + 2);
                if (*(short *)sub & 0x800) {
                    sub = sub + 0x24 + cnt * 0x30;
                } else {
                    sub = sub + 0x14 + cnt * 0x18;
                }
            }
            if (r != 0) {
                break;
            }
            subN++;
        }

        if (r != 0) {
            ok = 0;
            *(float *)0x700031A4 = *(float *)0x700031B4;
            *(void **)0x700031D4 = 0;
            t1 = *(float *)0x700031A4;
            *(short *)0x700030CA = (*(short *)0x700030CA & 0xFF00) | surf;
            if (t0 <= t1) {
                lo = t0;
                hi = t1;
            } else {
                lo = t1;
                hi = t0;
            }
        }
    }

    objCount = D_00275B84;
    for (obj = 0; obj < objCount; obj++) {
        e = D_00275B7C[obj];
        if (e[0] == 0 || (e[2] & 0x1F) != 4 || (char *)e == *(char **)0x70003254) {
            continue;
        }
        hitIdx = (int)(short)(*(unsigned short *)(e + 0xE) >> 8) & 0xFF;
        if (hitIdx == 0xFF) {
            continue;
        }
        hitFlags = *(int *)(base + hitIdx * 4 + 4);
        if (hitFlags == 0 || hitIdx < 0 || hitIdx >= *(short *)0x7000324C) {
            continue;
        }
        hitPrim = base + hitFlags;
        if (lo < *(float *)(hitPrim + 4) || hi > *(float *)(hitPrim + 0x10)) {
            continue;
        }
        if (*(float *)0x70003190 < *(float *)(hitPrim + 0) || *(float *)0x70003190 > *(float *)(hitPrim + 0xC)) {
            continue;
        }
        if (*(float *)0x70003198 < *(float *)(hitPrim + 8) || *(float *)0x70003198 > *(float *)(hitPrim + 0x14)) {
            continue;
        }

        subCount = *(short *)(hitPrim + 0x18);
        sub = hitPrim + 0x18 + 4;
        r = 0;
        subN = 0;
        while (subN < subCount) {
            kind = *(short *)sub & 0xF000;
            if (kind == 0x8000) {
                r = func_001A44B0(sub);
                if (*(short *)sub & 0x800) {
                    sub += 0x24;
                } else {
                    sub += 0x14;
                }
            } else if (kind == 0x4000) {
                r = func_001A44B0(sub);
                if (*(short *)sub & 0x800) {
                    sub += 0x2C;
                } else {
                    sub += 0x18;
                }
            } else if (kind == 0x2000) {
                r = func_001A4650(sub);
                sub += 0x1C;
            } else if (kind == 0x1000) {
                r = func_001A4030(sub);
                cnt = *(unsigned char *)(sub + 2);
                if (*(short *)sub & 0x800) {
                    sub = sub + 0x24 + cnt * 0x30;
                } else {
                    sub = sub + 0x14 + cnt * 0x18;
                }
            }
            if (r != 0) {
                break;
            }
            subN++;
        }

        if (r != 0) {
            ok = 0;
            *(float *)0x700031A4 = *(float *)0x700031B4;
            *(unsigned char **)0x700031D4 = e;
            t1 = *(float *)0x700031A4;
            *(short *)0x700030CA = (*(short *)0x700030CA & 0xFF00) | e[0x54];
            if (t0 <= t1) {
                lo = t0;
                hi = t1;
            } else {
                lo = t1;
                hi = t0;
            }
        }
    }

    return ok;
}
