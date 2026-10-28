// NEARMISS func_001A06A0  (vram 0x001A06A0, 0x46C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The body follows the original instructions;
// the residual diff is code generation only:
// Body corrected 2026-09-25 against the original instructions (a list entry is skipped when its FIRST BYTE is 0, not when the pointer is null; min/max names fixed; was 86.42%).
// Matching wave 2026-10-27 (was 87.11%): the probe-point block 0x70003190 and the words 0x700030B0 / 0x700030CA are relocated externs (lets the
// final loads move above the 0x700030CA store); the list is walked with a pointer (`*list++`); the variable-size records advance as typed record
// pointers; every min/max pair is computed in the same if/else shape; failures are early `continue`s; the box count is a `short *`.
// Remaining differences: FP register colouring only — the x-axis max always lands in $f25 (target $f23), with the z max in $f23 (target $f25);
// not moved by declaration order, statement/block order, box-test order, temps, renaming, or a 420 s 2.3.3 permuter run.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern int func_001A4030(void *p);
extern int func_001A50A0(void *p);
extern int func_001A5C30(void *p);

typedef struct { char b[0x30]; } Rec30;
typedef struct { char b[0x18]; } Rec18;

extern char **D_00275B7C;
extern short D_00275B84;
extern char D_700030B0[];
extern float D_70003190[];
extern short D_700030CA[];

int func_001A06A0(void) {
    float minx, miny, minz, maxz, maxx, maxy;
    int i;
    int j;
    int result;
    short *cnt;
    char *cur;
    char *e;
    char **list;
    int idx;
    int off;
    int base;
    char *box;
    int hit;
    float t;

    *(void **)0x700031D0 = D_700030B0;

    maxx = D_70003190[0];
    t = D_70003190[4];
    result = 1;
    if (!(maxx <= t)) {
        minx = t;
    } else {
        minx = maxx;
        maxx = t;
    }

    maxy = D_70003190[1];
    t = D_70003190[5];
    if (!(maxy <= t)) {
        miny = t;
    } else {
        miny = maxy;
        maxy = t;
    }

    maxz = D_70003190[2];
    t = D_70003190[6];
    if (!(maxz <= t)) {
        minz = t;
    } else {
        minz = maxz;
        maxz = t;
    }

    list = D_00275B7C;
    for (i = 0; i < D_00275B84; i++) {
        e = *list++;
        if (*(unsigned char *)e == 0) continue;
        if ((*(unsigned char *)(e + 2) & 0x1F) != 4) continue;
        if ((char *)*(int *)0x70003254 == e) continue;
        idx = (*(unsigned short *)(e + 0xE) >> 8) & 0xFF;
        if (idx == 0xFF) continue;
        base = *(int *)0x70003250;
        off = *(int *)(base + idx * 4 + 4);
        if (off == 0) continue;
        if (idx >= *(short *)0x7000324C || idx < 0) continue;
        box = (char *)(base + off);
        if (maxx < *(float *)(box + 0)) continue;
        if (!(minx <= *(float *)(box + 0xC))) continue;
        if (maxz < *(float *)(box + 8)) continue;
        if (!(minz <= *(float *)(box + 0x14))) continue;
        if (maxy < *(float *)(box + 4)) continue;
        if (!(miny <= *(float *)(box + 0x10))) continue;
        if (*(unsigned char *)(e + 0x54) >= 0x50) continue;
        cnt = (short *)(box + 0x18);
        cur = (char *)cnt + 4;
        hit = 0;
        for (j = 0; j < *cnt; j++) {
            switch (*(short *)cur & 0xF000) {
            case 0x8000:
                hit = func_001A5C30(cur);
                if (*(short *)cur & 0x800) {
                    cur += 0x24;
                } else {
                    cur += 0x14;
                }
                break;
            case 0x4000:
                hit = func_001A5C30(cur);
                if (*(short *)cur & 0x800) {
                    cur += 0x2C;
                } else {
                    cur += 0x18;
                }
                break;
            case 0x2000:
                hit = func_001A50A0(cur);
                cur += 0x1C;
                break;
            case 0x1000:
                hit = func_001A4030(cur);
                if (*(short *)cur & 0x800) {
                    cur = (char *)((Rec30 *)(cur + 0x24) + *(unsigned char *)(cur + 2));
                } else {
                    cur = (char *)((Rec18 *)(cur + 0x14) + *(unsigned char *)(cur + 2));
                }
                break;
            }
            if (hit != 0) break;
        }
        if (hit == 0) continue;
        {
            float *p = D_70003190;
            int k = 0;
            do {
                k += 1;
                p[4] = p[8];
                p += 1;
            } while (k < 3);
        }
        *(char **)0x700031D4 = e;
        result = 0;
        D_700030CA[0] = (short)((D_700030CA[0] & 0xFF00) | (*(unsigned char *)(e + 0x54) & 0xFF));
        if (!(D_70003190[0] <= D_70003190[4])) {
            minx = D_70003190[4];
        } else {
            maxx = D_70003190[4];
        }
        if (!(D_70003190[1] <= D_70003190[5])) {
            miny = D_70003190[5];
        } else {
            maxy = D_70003190[5];
        }
        if (!(D_70003190[2] <= D_70003190[6])) {
            minz = D_70003190[6];
        } else {
            maxz = D_70003190[6];
        }
    }
    return result;
}
