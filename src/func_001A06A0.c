// NEARMISS func_001A06A0  (vram 0x001A06A0, 0x46C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.42% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Full AABB-narrowing scan + hitbox-type dispatch logic recovered (types 0x1000/0x2000/0x4000/0x8000 -> func_001A4030/001A50A0/001A5C30). Required -sdatathreshold 8 to gp-relativize D_00275B7C/D_00275B84 (matching byte-matched sibling func_001AA700); the numerous 0x7000xxxx scratch globals kept as ...
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

extern char **D_00275B7C;
extern short D_00275B84;

int func_001A06A0(void) {
    float lox, hix;
    float loy, hiy;
    float loz, hiz;
    char **list;
    int i;
    int result;
    char *e;
    int idx;
    int base;
    int off;
    char *box;
    char *cur;
    int j;
    int hit;
    float t;

    *(void **)0x700031D0 = (void *)0x700030B0;

    lox = *(float *)0x70003190;
    t = *(float *)0x700031A0;
    if (!(lox <= t)) {
        hix = t;
    } else {
        hix = lox;
        lox = t;
    }

    loy = *(float *)0x70003194;
    t = *(float *)0x700031A4;
    hiy = loy;
    if (!(loy <= t)) {
        hiy = t;
    } else {
        loy = t;
    }

    loz = *(float *)0x70003198;
    t = *(float *)0x700031A8;
    hiz = loz;
    if (!(loz <= t)) {
        hiz = t;
    } else {
        loz = t;
    }

    list = D_00275B7C;
    i = 0;
    result = 1;
    while (i < D_00275B84) {
        e = list[i];
        if (e != 0 && (*(unsigned char *)(e + 2) & 0x1F) == 4 && (char *)*(int *)0x70003254 != e) {
            idx = (*(unsigned short *)(e + 0xE) >> 8) & 0xFF;
            if (idx != 0xFF) {
                base = *(int *)0x70003250;
                off = *(int *)(base + idx * 4 + 4);
                if (off != 0 && idx < *(short *)0x7000324C && idx >= 0) {
                    box = (char *)(base + off);
                    if (!(lox < *(float *)(box + 0)) && hix <= *(float *)(box + 0xC)
                        && !(loz < *(float *)(box + 8)) && hiz <= *(float *)(box + 0x14)
                        && !(loy < *(float *)(box + 4)) && hiy <= *(float *)(box + 0x10)
                        && (*(unsigned char *)(e + 0x54) < 0x50)) {
                        cur = box + 0x18 + 4;
                        hit = 0;
                        j = 0;
                        while (j < *(short *)(box + 0x18)) {
                            switch (*(unsigned short *)cur & 0xF000) {
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
                                    cur = cur + 0x24 + *(unsigned char *)(cur + 2) * 0x30;
                                } else {
                                    cur = cur + 0x14 + *(unsigned char *)(cur + 2) * 0x18;
                                }
                                break;
                            }
                            if (hit == 0) {
                                j += 1;
                                continue;
                            }
                            break;
                        }
                        if (hit != 0) {
                            {
                                float *p = (float *)0x70003190;
                                int k = 0;
                                do {
                                    k += 1;
                                    p[4] = p[8];
                                    p += 1;
                                } while (k < 3);
                            }
                            *(char **)0x700031D4 = e;
                            result = 0;
                            *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | *(unsigned char *)(e + 0x54));
                            t = *(float *)0x700031A0;
                            if (!(*(float *)0x70003190 <= t)) {
                                hix = t;
                            } else {
                                lox = t;
                            }
                            t = *(float *)0x700031A4;
                            if (!(*(float *)0x70003194 <= t)) {
                                hiy = t;
                            } else {
                                loy = t;
                            }
                            t = *(float *)0x700031A8;
                            if (!(*(float *)0x70003198 <= t)) {
                                hiz = t;
                            } else {
                                loz = t;
                            }
                        }
                    }
                }
            }
        }
        i += 1;
    }
    return result;
}
