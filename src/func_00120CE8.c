// NEARMISS func_00120CE8  (vram 0x00120CE8, 0x254 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 75.62% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-allocation-ORDER permutation across the whole body (target holds -1 in s5, &D_00242978[8] in s8; ee-gcc colors a shifted register set) plus adjacent-store scheduling. Correct compiler is ee-gcc (mwcc caps ~31-34% because it cannot emit the daddu/ld/dsll32 64-bit idioms or the term...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00122C48(int arg0, int size);
extern void func_001200E8(int arg0, char *p);
extern char D_00242978[];
extern long long D_00242D88;
extern int D_00242D90;
extern unsigned int D_00242D98;
extern unsigned int D_00242DA0;
extern int D_00242DA8;

void func_00120CE8(int arg0, unsigned int arg1) {
    char **headp;
    char *head;
    int hsize;
    int usable;
    int need;
    int base;
    int pad;
    int extra;
    int got;
    int off;

    headp = (char **)(D_00242978 + 8);
    head = *headp;
    hsize = *(int *)(head + 4);
    usable = hsize & 0xFFFFFFFC;
    need = (int)((long long)arg1 + D_00242D88 + 0x10);
    base = (int)(head + usable);
    if (D_00242D90 != -1) {
        need = (need + 0xFFF) & ~0xFFF;
    }
    got = func_00122C48(arg0, need);
    if (got == -1) {
        return;
    }
    if ((unsigned int)got >= (unsigned int)base || head != (char *)D_00242978) {
        D_00242DA8 = D_00242DA8 + need;
        if (got == base) {
            *(int *)(*headp + 4) = (need + usable) | 1;
        } else {
            if (D_00242D90 == -1) {
                D_00242D90 = got;
            } else {
                D_00242DA8 = D_00242DA8 + (got - base);
            }
            off = (got + 8) & 0xF;
            if (off != 0) {
                pad = 0x10 - off;
                got = got + pad;
            } else {
                pad = 0;
            }
            extra = pad + (int)(0x1000 - ((got + need) & 0xFFF));
            base = func_00122C48(arg0, extra);
            if (base == -1) {
                return;
            }
            D_00242DA8 = D_00242DA8 + extra;
            *(int *)(got + 4) = ((base - got) + extra) | 1;
            *headp = (char *)got;
            if (head != (char *)D_00242978) {
                if ((unsigned int)usable < 0x10) {
                    *(int *)(got + 4) = 1;
                    return;
                }
                usable = (usable - 0xC) & 0xFFFFFFF0;
                *(int *)(head + 4) = (*(int *)(head + 4) & 1) | usable;
                *(int *)(head + usable + 8) = 5;
                *(int *)(head + usable + 4) = 5;
                if ((unsigned int)usable >= 0x10) {
                    func_001200E8(arg0, head + 8);
                }
            }
        }
        if (D_00242D98 < (unsigned int)D_00242DA8) {
            D_00242D98 = (unsigned int)D_00242DA8;
        }
        if (D_00242DA0 < (unsigned int)D_00242DA8) {
            D_00242DA0 = (unsigned int)D_00242DA8;
        }
    }
}
