// NEARMISS func_00107E88  (vram 0x00107E88, 0x118 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.64% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc sibling-call wall. Expected emits a true tail call to func_00107CB8 (full epilogue then `j func_00107CB8`, with `daddu a0,s2,zero` materialized early as the sibling-call arg). Our ee-gcc 2.9-991111-01 refuses to sibling-call this shape (the tail call sits after an if/else merge), emitting `jal func_00107CB8` i...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00107CF0(int a0, int a1, int a2, int a3);
extern int func_00107A28(void);
extern void func_001084B0(int a0);
extern void func_00107AC8(int a0);
extern int func_00107CB8(int a0);

extern int D_002412F4;
extern unsigned int D_002411E0[];

int func_00107E88(int a0) {
    int g = D_002412F4;
    int s1 = *(int *)(g + 0x40);
    func_00107CF0(a0, g + 0x10, g + 0x18, g + 0x20);
    g = D_002412F4;
    *(int *)(s1 + 0x80) = *(int *)(g + 0x10);
    *(unsigned long *)(s1 + 0x88) =
        D_002411E0[(int)(((long)*(long *)(g + 0x20) << 27) >> 32) & 0xF];
    *(int *)(s1 + 0xCC) = *(int *)(a0 + 0x5C);
    *(int *)(s1 + 0xD0) = *(int *)(a0 + 0x60);
    *(int *)(s1 + 0xB4) = *(int *)(a0 + 0x44);
    *(int *)(s1 + 0xB8) = *(int *)(a0 + 0x48);
    *(int *)(s1 + 0xBC) = *(int *)(a0 + 0x4C);
    *(int *)(s1 + 0xC0) = *(int *)(a0 + 0x50);
    *(int *)(s1 + 0xC4) = *(int *)(a0 + 0x54);
    *(int *)(s1 + 0xC8) = *(int *)(a0 + 0x58);
    if (func_00107A28() != 0) {
        if (*(int *)(a0 + 0x28) == 1) {
            if (*(int *)(s1 + 0xB0) != 0) {
                func_001084B0(a0);
            } else {
                func_00107AC8(a0);
            }
            return func_00107CB8(a0);
        }
    }
    return 1;
}
