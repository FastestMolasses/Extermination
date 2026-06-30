// NEARMISS func_001FABF0  (vram 0x001FABF0, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Parameter-save-order permutation. CW target saves arg2(scale)->s0 before the 'beqz a3' guard and fills the delay slot with arg0(idx)->s1; mwcc 2.3.3 does the reverse (idx->s1 before, scale->s0 in slot), cascading one trailing nop that shifts the shared epilogue branch targets 0x164->0x168. Every ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char D_00282154[];
extern int D_00282028[];
extern long long D_00281FD8[];
extern int D_0028202C[];
extern int D_00282024[];

extern void func_001FA790(void);
extern void func_0011A608(long long a, int b, int c);

void func_001FABF0(int idx, int a1, int scale, int enable) {
    int off;
    int s = scale;

    if (enable != 0) {
        if (*(char *)((char *)D_00282154 + idx) == 0) {
            off = idx * 0x60;
            *(int *)((char *)D_00282028 + off) = 0;
            func_001FA790();
            func_0011A608(*(long long *)((char *)D_00281FD8 + off), 0, 0);
            goto common;
        }
        return;
    }
    if (*(char *)((char *)D_00282154 + idx) == 0) {
        return;
    }
common:
    off = idx * 0x60;
    *(char *)((char *)D_0028202C + off) = 0;
    if (s != 0) {
        if (idx != 0) {
            *(float *)((char *)D_00282024 + off) = 16383.0f / (float)s;
            return;
        }
        *(float *)((char *)D_00282024 + off) = 16383.0f / (float)s;
        return;
    }
    if (idx != 0) {
        *(int *)((char *)D_00282024 + off) = 0x467FFC00;
        return;
    }
    *(int *)((char *)D_00282024 + off) = 0x467FFC00;
}
