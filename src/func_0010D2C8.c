// NEARMISS func_0010D2C8  (vram 0x0010D2C8, 0x5C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.23% via ee-gcc 2.9-991111-01 (-O2) (was 88.01%; 81.71% before 2026-10-27). Wave
// 2026-10-27 fixes read from the original instructions: the format pointer is ONE variable
// advanced by the loops' increments (outer `p++`, inner `for (;;) { p++; switch (*p) ... }`),
// with a separate buffer pointer `s`; one 64-bit value variable shared by every conversion;
// the second width digit is read before the first digit is tested; func_00128350 returns a
// double (__extendsfdf2) and func_0010D160 takes it; 64-bit div/mod are explicit helper calls.
// Wave 2026-10-28: the hex digit store is an if/else with one store per arm (not a ternary).
// Remaining differences: saved-register swap (the original keeps the buffer pointer in the
// outer pointer's register and the value in the next one; declaration order and a separate
// outer/inner pointer pair do not move it), the 'e'/'f' zero test lays out the '0' branch
// first, the 's' case tests the string in a temporary before copying it to the buffer
// pointer, one temp-register choice in the width parse, and the .text is 8 bytes short.
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
//   'd'         signed radix 10 (emits '-' then negates; uses the libgcc 64-bit helpers
//               func_001258E0 = __moddi3 / func_00125068 = __divdi3; 'u' uses the
//               unsigned pair func_00126578 / func_00125FA8).
//   'e' 'f'     float: prints '0' when exactly 0.0f, else widens it to double
//               (func_00128350 = __extendsfdf2) and prints it with func_0010D160.
//   's'         string, or the literal "(null)" when the first byte is 0.
//   'c'         single character.
// Any other character after '%' is skipped silently.

extern void func_0010D160(double x);
extern long long func_00125068(long long a, long long b);
extern long long func_001258E0(long long a, long long b);
extern unsigned long long func_00125FA8(unsigned long long a, unsigned long long b);
extern unsigned long long func_00126578(unsigned long long a, unsigned long long b);
extern double func_00128350(float f);

extern void (*D_00241C74)(int c);

void func_0010D2C8(char *p, char *ap)
{
    char buf[32];
    char *s;
    char *pad;
    int lmod;
    unsigned long long uv;

    for (; *p; p++) {
        pad = 0;
        lmod = 0;
        if (*p == '%') {
            for (;;) {
                p++;
                switch (*p) {
                case '0': {
                    int n = p[1] - '0';
                    int c2 = p[2];

                    if ((unsigned char)n < 10) {
                        if ((unsigned int)(c2 - '0') < 10) {
                            n = n * 10 + (c2 - '0');
                            if (n >= 32) {
                                n = 31;
                            }
                            p += 2;
                        } else {
                            p += 1;
                        }
                        pad = buf + (31 - n);
                        while (n > 0) {
                            buf[31 - n] = '0';
                            n--;
                        }
                    }
                    continue;
                }
                case 'l':
                    lmod = 'l';
                    continue;
                case 'h':
                    lmod = 'h';
                    continue;
                case 'o': {
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
                    s = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        s = &buf[30];
                        buf[30] = '0';
                    } else {
                        while (uv != 0) {
                            *--s = (uv & 7) + '0';
                            uv >>= 3;
                        }
                    }
                    if (pad != 0) {
                        if (pad < s) {
                            s = pad;
                        }
                    }
                    while (*s) {
                        (*D_00241C74)(*s);
                        s++;
                    }
                    break;
                }
                case 'x': {
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
                    s = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        s = &buf[30];
                        buf[30] = '0';
                    } else {
                        while (uv != 0) {
                            unsigned long long d = uv & 0xF;

                            if (d < 10) {
                                *--s = d + '0';
                            } else {
                                *--s = d + 0x57;
                            }
                            uv >>= 4;
                        }
                    }
                    if (pad != 0) {
                        if (pad < s) {
                            s = pad;
                        }
                    }
                    while (*s) {
                        (*D_00241C74)(*s);
                        s++;
                    }
                    break;
                }
                case 'd': {
                    if (lmod == 'l') {
                        ap += 8;
                        uv = *(long long *)(ap - 8);
                    } else if (lmod == 'h') {
                        ap += 8;
                        uv = *(short *)(ap - 8);
                    } else {
                        ap += 8;
                        uv = *(int *)(ap - 8);
                    }
                    s = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        s = &buf[30];
                        buf[30] = '0';
                    } else {
                        if ((long long)uv < 0) {
                            uv = -uv;
                            (*D_00241C74)('-');
                        }
                        while (uv != 0) {
                            *--s = func_001258E0(uv, 10) + '0';
                            uv = func_00125068(uv, 10);
                        }
                    }
                    if (pad != 0) {
                        if (pad < s) {
                            s = pad;
                        }
                    }
                    while (*s) {
                        (*D_00241C74)(*s);
                        s++;
                    }
                    break;
                }
                case 'u': {
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
                    s = &buf[31];
                    buf[31] = 0;
                    if (uv == 0) {
                        s = &buf[30];
                        buf[30] = '0';
                    } else {
                        while (uv != 0) {
                            *--s = func_00126578(uv, 10) + '0';
                            uv = func_00125FA8(uv, 10);
                        }
                    }
                    if (pad != 0) {
                        if (pad < s) {
                            s = pad;
                        }
                    }
                    while (*s) {
                        (*D_00241C74)(*s);
                        s++;
                    }
                    break;
                }
                case 'e':
                case 'f': {
                    float f;

                    ap += 8;
                    f = *(float *)(ap - 8);
                    if (f != 0.0f) {
                        func_0010D160(func_00128350(f));
                    } else {
                        (*D_00241C74)('0');
                    }
                    break;
                }
                case 's': {
                    ap += 8;
                    s = *(char **)(ap - 8);
                    if (*s != 0) {
                        do {
                            (*D_00241C74)(*s);
                            s++;
                        } while (*s);
                    } else {
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
                    ap += 8;
                    uv = *(char *)(ap - 8);
                    (*D_00241C74)((int)uv);
                    break;
                }
                default:
                    break;
                }
                break;
            }
        } else {
            (*D_00241C74)(*p);
        }
    }
}
