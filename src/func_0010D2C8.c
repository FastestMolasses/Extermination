// NEARMISS func_0010D2C8  (vram 0x0010D2C8, 0x5C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.71% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NOT an mwcc unit — this is an ee-gcc (SDK/lowmem) function: sd/ld 64-bit callee saves at 16-byte stride, daddu register moves, movz/movn, dsrl/dsra32, 3-operand mult, unfilled jal;nop. Built with tools/eegcc/ee-compile.sh -O2 (mwcc cannot match it at all; -O1 gives 62.4%, -O3/-fno-strength-reduce...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// SEMANTICS: vprintf core. Walks format string `p`, emitting each byte through the
// global callback D_00241C74(int). `ap` is a raw varargs cursor advanced 8 bytes per
// conversion. Supported directives (jtbl_0026B880, 73 entries indexed by *p - '0'):
//   '0'<width>  zero-pad flag: parses 1 or 2 decimal digits (clamped to 31), pre-fills
//               buf[31-n..30] with '0' and sets `pad` = &buf[31] - n, so the later
//               "if (pad && pad < s) s = pad" step widens the printed field.
//   'l' / 'h'   length modifiers (recorded in `lmod` as the literal chars 'l' / 'h').
//   'o' 'x' 'u' unsigned radix 8 / 16 / 10, built backwards from &buf[31].
//   'd'         signed radix 10 (emits '-' then negates; uses the 64-bit div/mod
//               helpers func_001258E0 / func_00125068).
//   'e' 'f'     float: prints '0' when exactly 0.0f, else func_0010D160(func_00128350(f)).
//   's'         string, or the literal "(null)" when the first byte is 0.
//   'c'         single character.
// Any other character after '%' is skipped silently.

extern void func_0010D160(char *s);
extern long long func_00125068(long long a, long long b);
extern long long func_001258E0(long long a, long long b);
extern unsigned long long func_00125FA8(unsigned long long a, unsigned long long b);
extern unsigned long long func_00126578(unsigned long long a, unsigned long long b);
extern char *func_00128350(float f);

extern void (*D_00241C74)(int c);

void func_0010D2C8(char *p, char *ap)
{
    char buf[32];
    char *q;
    char *pad;
    int lmod;

    while (*p) {
        pad = 0;
        lmod = 0;
        if (*p == '%') {
            q = p + 1;
            for (;;) {
                p = q;
                switch (*p) {
                case '0': {
                    int n = p[1] - '0';

                    if ((unsigned char)n < 10) {
                        if ((unsigned int)(p[2] - '0') < 10) {
                            n = n * 10 + (p[2] - '0');
                            if (n >= 32) {
                                n = 31;
                            }
                            p += 2;
                        } else {
                            p += 1;
                        }
                        pad = &buf[31] - n;
                        while (n > 0) {
                            buf[31 - n] = '0';
                            n--;
                        }
                        q = p + 1;
                    } else {
                        q++;
                    }
                    continue;
                }
                case 'l':
                    lmod = 'l';
                    q++;
                    continue;
                case 'h':
                    lmod = 'h';
                    q++;
                    continue;
                case 'o': {
                    unsigned long long uv;

                    if (lmod == 'l') {
                        ap += 8;
                        uv = *(unsigned long long *)(ap - 8);
                    } else if (lmod == 'h') {
                        ap += 8;
                        uv = *(unsigned short *)(ap - 8);
                    } else {
                        ap += 8;
                        uv = *(unsigned int *)(ap - 8);
                    }
                    p = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        p = &buf[30];
                        buf[30] = '0';
                        q++;
                    } else {
                        q++;
                        while (uv != 0) {
                            *--p = (uv & 7) + '0';
                            uv >>= 3;
                        }
                    }
                    if (pad != 0) {
                        if (pad < p) {
                            p = pad;
                        }
                    }
                    while (*p) {
                        (*D_00241C74)(*p);
                        p++;
                    }
                    break;
                }
                case 'x': {
                    unsigned long long uv;

                    if (lmod == 'l') {
                        ap += 8;
                        uv = *(unsigned long long *)(ap - 8);
                    } else if (lmod == 'h') {
                        ap += 8;
                        uv = *(unsigned short *)(ap - 8);
                    } else {
                        ap += 8;
                        uv = *(unsigned int *)(ap - 8);
                    }
                    p = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        p = &buf[30];
                        buf[30] = '0';
                        q++;
                    } else {
                        q++;
                        while (uv != 0) {
                            unsigned int d = uv & 0xF;

                            *--p = (d < 10) ? (d + '0') : (d + 0x57);
                            uv >>= 4;
                        }
                    }
                    if (pad != 0) {
                        if (pad < p) {
                            p = pad;
                        }
                    }
                    while (*p) {
                        (*D_00241C74)(*p);
                        p++;
                    }
                    break;
                }
                case 'd': {
                    long long sv;

                    if (lmod == 'l') {
                        ap += 8;
                        sv = *(long long *)(ap - 8);
                    } else if (lmod == 'h') {
                        ap += 8;
                        sv = *(short *)(ap - 8);
                    } else {
                        ap += 8;
                        sv = *(int *)(ap - 8);
                    }
                    p = &buf[31];
                    buf[31] = 0;
                    if (sv == 0) {
                        p = &buf[30];
                        buf[30] = '0';
                        q++;
                    } else {
                        if (sv < 0) {
                            sv = -sv;
                            (*D_00241C74)('-');
                        }
                        q++;
                        while (sv != 0) {
                            *--p = (sv % 10) + '0';
                            sv = sv / 10;
                        }
                    }
                    if (pad != 0) {
                        if (pad < p) {
                            p = pad;
                        }
                    }
                    while (*p) {
                        (*D_00241C74)(*p);
                        p++;
                    }
                    break;
                }
                case 'u': {
                    unsigned long long uv;

                    if (lmod == 'l') {
                        ap += 8;
                        uv = *(long long *)(ap - 8);
                    } else if (lmod == 'h') {
                        ap += 8;
                        uv = *(unsigned short *)(ap - 8);
                    } else {
                        ap += 8;
                        uv = *(unsigned int *)(ap - 8);
                    }
                    p = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        p = &buf[30];
                        buf[30] = '0';
                        q++;
                    } else {
                        q++;
                        while (uv != 0) {
                            *--p = (uv % 10) + '0';
                            uv = uv / 10;
                        }
                    }
                    if (pad != 0) {
                        if (pad < p) {
                            p = pad;
                        }
                    }
                    while (*p) {
                        (*D_00241C74)(*p);
                        p++;
                    }
                    break;
                }
                case 'e':
                case 'f': {
                    float f;

                    ap += 8;
                    f = *(float *)(ap - 8);
                    if (f != 0.0f) {
                        q++;
                        func_0010D160(func_00128350(f));
                    } else {
                        q++;
                        (*D_00241C74)('0');
                    }
                    break;
                }
                case 's': {
                    ap += 8;
                    p = *(char **)(ap - 8);
                    if (*p != 0) {
                        q++;
                        do {
                            (*D_00241C74)(*p);
                            p++;
                        } while (*p);
                    } else {
                        q++;
                        (*D_00241C74)('(');
                        (*D_00241C74)('n');
                        (*D_00241C74)('u');
                        (*D_00241C74)('l');
                        (*D_00241C74)('l');
                        (*D_00241C74)(')');
                    }
                    break;
                }
                case 'c': {
                    long long sv;

                    ap += 8;
                    sv = *(char *)(ap - 8);
                    q++;
                    (*D_00241C74)((int)sv);
                    break;
                }
                default:
                    q++;
                    break;
                }
                break;
            }
            p = q;
        } else {
            q = p + 1;
            (*D_00241C74)(*p);
            p = q;
        }
    }
}
