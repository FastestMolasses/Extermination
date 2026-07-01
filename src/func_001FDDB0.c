// NEARMISS func_001FDDB0  (vram 0x001FDDB0, 0x2B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Multiple scheduling/branch-shape artifacts on an otherwise byte-faithful 96-instruction body (fully recovered subtitle/HUD text-record interpreter): (1) mwcc merges source 'case 0' into 'default' (identical target, adjacent to 'case 32: return 0;'), collapsing target's distinct beqz-vs-zero test ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Subtitle/HUD text-record interpreter. arg0 = array of command-record pointers (NULL-terminated
// runs mark literal-glyph slots), arg1 = record count limit, arg2 = a text-layout context: +0x20
// wrap-width, +0x24/+0x28 running pen x/y (rewound by the accumulated glyph width 'col' on each
// flush), +0x2C source glyph-byte array, +0x40/+0x48/+0x58 a "busy" counter/mode/countdown for
// command 1 (an animated reveal-gate: mode 0 arms a countdown from rec[1] and fires
// func_001FB9F0(0x8C9,...) once; mode 1 ticks the countdown to 0 then advances +0x40). Commands:
// 1 = gate (above), 2 = set color D_00275C50 from palette D_0026EC10[rec[1]], 3 = set glyph scale
// D_00275C55 = rec[1]<<3, 4 = both (color from rec[1], scale from the record's own byte +0xC), 32 =
// abort (return 0). Any other/absent record emits the literal glyph at arg2->0x2C[i] into the
// 0x40-byte scratch line buffer D_00820F50 and accumulates its pixel width via func_001CC170
// (called on a stack-local 2-byte "char + NUL" buffer, its string-width convention). On flush
// (command dispatch when the buffer isn't NULL) func_001FC770 draws the accumulated line at the
// current pen position and the buffer/width reset. Returns 1 when the record limit or the 0x40-byte
// buffer cap is reached, 0 on an abort/gate-wait.

extern void func_00121A28(unsigned char *p, int v, int n);
extern int func_001CC170(unsigned char *p);
extern int func_001FB9F0(int a, int b, int c, int d);
extern void func_001FC770(int a0, int a1, unsigned char *a2, int a3);

extern int D_0026EC10[2];
extern int D_00275C50;
extern unsigned char D_00275C55;
extern unsigned char D_00820F50[0x40];

int func_001FDDB0(int **arg0, int arg1, char *arg2) {
    int i;
    int buf;
    int col;
    int base;
    int cmd;
    int *rec;
    unsigned char c[2];

    base = *(int *)(arg2 + 0x24);
    cmd = *(int *)(arg2 + 0x28);
    col = 0;
    func_00121A28(D_00820F50, 0, 0x40);
    i = 0;
    buf = 0;
    goto test;

    for (;;) {
        if (*arg0 != 0) {
            base = base + col;
            func_001FC770(base, cmd, D_00820F50, *(int *)(arg2 + 0x20));
            col = 0;
            buf = 0;
            func_00121A28(D_00820F50, 0, 0x40);

            rec = *arg0;
            switch (rec[0]) {
            case 1:
                if ((unsigned int)*(int *)(arg2 + 0x40) <= (unsigned int)rec[2]) {
                    switch (*(int *)(arg2 + 0x48)) {
                    case 0:
                        *(int *)(arg2 + 0x58) = rec[1];
                        *(int *)(arg2 + 0x40) = (*arg0)[2];
                        if (*(int *)(arg2 + 0x58) == 0) {
                            *(int *)(arg2 + 0x40) = *(int *)(arg2 + 0x40) + 1;
                            goto next;
                        }
                        *(int *)(arg2 + 0x48) = 1;
                        func_001FB9F0(0x8C9, 0x1000, 0x1000, 0x1000);
                        /* fall through */
                    default:
                        return 0;
                    case 1:
                        *(int *)(arg2 + 0x58) = *(int *)(arg2 + 0x58) - 1;
                        if (*(int *)(arg2 + 0x58) <= 0) {
                            *(int *)(arg2 + 0x48) = 0;
                            *(int *)(arg2 + 0x58) = 0;
                            *(int *)(arg2 + 0x40) = *(int *)(arg2 + 0x40) + 1;
                            goto next;
                        }
                        return 0;
                    }
                }
                break;
            case 2:
                D_00275C50 = D_0026EC10[rec[1]];
                break;
            case 3:
                D_00275C55 = ((unsigned char)rec[1]) << 3;
                break;
            case 4:
                D_00275C50 = D_0026EC10[rec[1]];
                D_00275C55 = ((unsigned char *)*arg0)[0xC] << 3;
                break;
            case 32:
                return 0;
            default:
                goto emit;
            }
        } else {
        emit:
            D_00820F50[buf] = *(unsigned char *)(*(int *)(arg2 + 0x2C) + i);
            c[0] = *(unsigned char *)(*(int *)(arg2 + 0x2C) + i);
            c[1] = 0;
            col += func_001CC170((unsigned char *)c);
        }
    next:
        arg0 += 1;
        i += 1;
        buf += 1;
    test:
        if (arg1 >= i) {
            if (i < 0x40) {
                continue;
            }
        }
        return 1;
    }
}
