// NEARMISS func_00124F58  (vram 0x00124F58, 0x10C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.03% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// EE-GCC function, not mwcc: disasm has daddu register moves, sd/ld 8-byte saves, and pervasive branch-likely (bnel/beql/beqzl). mwcc emits paddub/sq and plain branches, so both mwcc builds cap ~57-59%. This is the ee-gcc-codegen park class for the mwcc wall. Correct compiler is ee-gcc 2.9-991111-0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00120058(char *);
extern void func_001200E8(char *, int);
extern void func_00120B98(char *);
extern char *D_0024295C;

int func_00124F58(char *arg0) {
    char *p;
    int v0;
    unsigned short fl;

    p = *(char **)(arg0 + 0x54);
    if (p != 0) {
        v0 = *(int *)(p + 0x38);
    } else {
        p = D_0024295C;
        *(char **)(arg0 + 0x54) = p;
        v0 = *(int *)(p + 0x38);
    }
    if (v0 == 0) {
        func_00120058(p);
    }
    fl = *(unsigned short *)(arg0 + 0xC);
    if (!(fl & 8)) {
        if (!(fl & 0x10)) {
            return -1;
        }
        if (fl & 4) {
            int sub = *(int *)(arg0 + 0x30);
            if (sub != 0) {
                if (sub != (int)(arg0 + 0x40)) {
                    func_001200E8(*(char **)(arg0 + 0x54), sub);
                }
                *(int *)(arg0 + 0x30) = 0;
            }
            fl = *(unsigned short *)(arg0 + 0xC) & ~0x24;
            *(int *)(arg0 + 4) = 0;
            *(short *)(arg0 + 0xC) = fl;
            *(int *)(arg0 + 0) = *(int *)(arg0 + 0x10);
        }
        *(short *)(arg0 + 0xC) = (short)(fl | 8);
    }
    if (*(int *)(arg0 + 0x10) == 0) {
        func_00120B98(arg0);
    }
    fl = *(unsigned short *)(arg0 + 0xC);
    if (fl & 1) {
        *(int *)(arg0 + 8) = 0;
        *(int *)(arg0 + 0x18) = -*(int *)(arg0 + 0x14);
    } else {
        v0 = 0;
        if (!(fl & 2)) {
            v0 = *(int *)(arg0 + 0x14);
        }
        *(int *)(arg0 + 8) = v0;
    }
    return 0;
}
