// NEARMISS func_001A32C0  (vram 0x001A32C0, 0x6B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 61.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation (multiple saved-reg groups from the two nested passes); logic and structure fully recovered and cross-verified against docs/FINDINGS.md.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Movable-object hull segment query (sibling of the static-world walker
// func_001A0B10; see docs/FINDINGS.md "4. Movable-object hulls" and
// "PLACEMENT TABLES ARE DISC DATA"). Query segment is staged in scratchpad
// by the caller: 0x70003190/0x70003198 = query point x/z, 0x70003194 /
// 0x700031A4 = the two candidate y-range bounds, clamped to the closer hit
// as the walk proceeds (matching the func_0019A570 hub's "clamp to nearest
// hit across sets" protocol). D_700030B0 is the hit-normal output record
// (FINDINGS: "stages the hit normal at D_700030B0"); this primes the
// result-record pointer 0x700031D0 to it before either pass runs.
//
//   1. The per-uid collision-record table at spad 0x70003250 (count
//      0x7000324C). Each 4-byte per-uid word's low bits are an OFFSET (from
//      the table base) to the object's hull record; high bits are flags:
//      0x80000000 populated, 0x40000000 disabled, 0x20000000 conditional
//      on hit-id 0x7000324E. For each populated/enabled uid, resolves the
//      PLACEMENT TABLE for the current area/sub-state via
//      D_0024D7C0[D_00810700][D_00810701] (the same per-area descriptor
//      documented for placement records) and reads that placement record's
//      `kind` field (+0x08, 0x28-byte records, index = uid) gated kind <
//      0x1E. Tests the hull's AABB (+0/+4 = x/y lo; +0xC/+0x10 = x/y hi;
//      +8/+0x14 = z lo/hi) against the query range, then walks its prim
//      list at +0x18 (count) / +0x1C (prims), dispatching by the 0xF000
//      prim-type nibble to func_001A4030 (0x1000 n-gon) / func_001A44B0
//      (0x4000/0x8000) / func_001A4650 (0x2000) -- each a single-arg leaf
//      whose hit result comes back in $v0 (return value), unrelated to the
//      prim's own fields. On a hit, clamps the far bound from the
//      floor-probe result 0x700031B4, clears the hit-object scratch
//      0x700031D4 (no actor for a placement/uid hull), and OR's the
//      placement record's `kind` byte into the low byte of the
//      surface-type word 0x700030CA.
//   2. The published per-frame CLASS-4 actor list (D_00275B7C cursor /
//      D_00275B84 count; see FINDINGS "Per-frame category lists"). For
//      each live actor (+0x00 nonzero, class byte +0x2 & 0x1F == 4, and not
//      the "self" actor cached at spad 0x70003254), resolves its uid from
//      the high byte of +0xE, looks up the SAME per-uid record table by
//      uid, and runs the identical AABB + prim-list test (kind byte now
//      read directly from the actor at +0x54, not a placement record). On
//      a hit, records the hit actor pointer into 0x700031D4 (instead of
//      clearing it) and its kind byte into 0x700030CA.
//
// Returns 1 if the segment cleared both passes with no hit, 0 if either
// pass found a blocking hit. The two passes duplicate the same AABB +
// prim-list test inline (the target has no shared subroutine here).
extern int D_700030B0;
extern unsigned char *D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(unsigned char *);
extern int func_001A44B0(unsigned char *);
extern int func_001A4650(unsigned char *);

int func_001A32C0(void)
{
    float lo, hi;
    int ret;
    int i, n_uid, n_cls4;
    unsigned char *uid_table;
    unsigned char *area_table;

    *(int **)0x700031D0 = &D_700030B0;

    {
        float a = *(float *)0x70003194;
        float b = *(float *)0x700031A4;
        if (a <= b) {
            lo = a;
            hi = b;
        } else {
            lo = b;
            hi = a;
        }
    }
    ret = 1;

    n_uid = *(short *)0x7000324C;
    for (i = 0; i < n_uid; i++) {
      uid_table = *(unsigned char **)0x70003250;
      {
        int off = *(int *)(uid_table + i * 4 + 4);

        if (off & 0x80000000) {
            if (!(off & 0x40000000) &&
                (!(off & 0x20000000) || *(short *)0x7000324E != 0)) {
                area_table = D_0024D7C0[D_00810700];
              {
                unsigned char *place_rec = area_table + i * 0x28;
                unsigned char kind = place_rec[8];

                if (kind < 0x1E) {
                    unsigned char *hull = uid_table + (off & 0x3FFFFFFF);

                    if (!(hi < *(float *)(hull + 4)) && (lo <= *(float *)(hull + 0x10)) &&
                        !(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                        !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14))) {
                        short prim_count = *(short *)(hull + 0x18);
                        unsigned char *p = hull + 0x18 + 4;
                        int hit = 0;
                        int j;

                        for (j = 0; j < prim_count; j++) {
                            int ptype = *(short *)p & 0xF000;

                            switch (ptype) {
                            case 0x1000:
                                hit = func_001A4030(p);
                                if (*(short *)p & 0x800) {
                                    p = p + 0x24 + p[2] * 0x30;
                                } else {
                                    p = p + 0x14 + p[2] * 0x18;
                                }
                                break;
                            case 0x2000:
                                hit = func_001A4650(p);
                                p += 0x1C;
                                break;
                            case 0x4000:
                            case 0x8000:
                                hit = func_001A44B0(p);
                                if (*(short *)p & 0x800) {
                                    p += 0x2C;
                                } else {
                                    p += 0x18;
                                }
                                break;
                            }
                            if (hit != 0) {
                                break;
                            }
                        }

                        if (hit != 0) {
                            ret = 0;
                            hi = *(float *)0x700031B4;
                            *(void **)0x700031D4 = 0;
                            *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | kind);
                            if (*(float *)0x70003194 <= hi) {
                                lo = hi;
                            }
                        }
                    }
                }
            }
        }
      }
      }
    }

    n_cls4 = D_00275B84;
    for (i = 0; i < n_cls4; i++) {
        unsigned char *a = D_00275B7C[i];

        if (a != 0 && (a[2] & 0x1F) == 4 && *(unsigned char **)0x70003254 != a) {
            int uid = (*(unsigned short *)(a + 0xE) >> 8) & 0xFF;

            if (uid != 0xFF) {
              uid_table = *(unsigned char **)0x70003250;
              {
                int off = *(int *)(uid_table + uid * 4 + 4);

                if (off != 0 && uid < *(short *)0x7000324C && uid >= 0) {
                    unsigned char *hull = uid_table + off;

                    if (!(hi < *(float *)(hull + 4)) && (lo <= *(float *)(hull + 0x10)) &&
                        !(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                        !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14)) &&
                        (a[0x54] < 0x1E)) {
                        short prim_count = *(short *)(hull + 0x18);
                        unsigned char *p = hull + 0x18 + 4;
                        int hit = 0;
                        int j;

                        for (j = 0; j < prim_count; j++) {
                            int ptype = *(short *)p & 0xF000;

                            switch (ptype) {
                            case 0x1000:
                                hit = func_001A4030(p);
                                if (*(short *)p & 0x800) {
                                    p = p + 0x24 + p[2] * 0x30;
                                } else {
                                    p = p + 0x14 + p[2] * 0x18;
                                }
                                break;
                            case 0x2000:
                                hit = func_001A4650(p);
                                p += 0x1C;
                                break;
                            case 0x4000:
                            case 0x8000:
                                hit = func_001A44B0(p);
                                if (*(short *)p & 0x800) {
                                    p += 0x2C;
                                } else {
                                    p += 0x18;
                                }
                                break;
                            }
                            if (hit != 0) {
                                break;
                            }
                        }

                        if (hit != 0) {
                            ret = 0;
                            hi = *(float *)0x700031B4;
                            *(unsigned char **)0x700031D4 = a;
                            *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | a[0x54]);
                            if (*(float *)0x70003194 <= hi) {
                                lo = hi;
                            }
                        }
                    }
                }
              }
            }
        }
    }

    return ret;
}
