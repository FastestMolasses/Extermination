// NEARMISS func_001FC280  (vram 0x001FC280, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Dead-temporary register coloring: the final pointer add (record + D_00810702*0x30) and its load (+0x20) use dest a0 under mwcc vs a1 in the target (two DIFF_ARG_MISMATCH, identical opcodes/operands otherwise). Body and structure are byte-equivalent. Pure regalloc-order artifact (permuter-class), ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 91.625% on the pinned mwcc (991202) build. Weapon/animation arm-select:
// indexes D_0024D650[D_00810700][D_00810701] to a record table, steps by
// D_00810702*0x30, and reads the packed word at +0x20: low half (id, signed >>16)
// and high half (lo, &0xFFFF). When the global weapon-id D_00810700==0xB and the
// sub-flag D_00810788==0xFF, force id=0x44E. If the cached active id D_00282160
// changed, tear down the old resource (func_0011A070(D_00282164) when prior id!=-1),
// store the new id, and for a valid (!=-1) id allocate via func_001FB9F0 with three
// 0x1000 budget params, caching the handle in D_00282164. Finally drive both arm
// slots: func_00119828(0, lo, lo) and func_00119828(1, lo, lo). Logic fully recovered.
// Sole residual: the final pointer add `record + D_00810702*0x30` is colored into a0
// by mwcc but a1 (the running accumulator) by the target -> two ARG_MISMATCH on a
// dead temp (addu/lw dest register). Register-allocation artifact (permuter-class),
// NOT the clean-store nop. -sdatathreshold 0 required (these globals are absolute).
extern void func_00119828(int, int, int);
extern void func_0011A070(int);
extern int func_001FB9F0(int, int, int, int);
extern int **D_0024D650[];
extern int D_00282160;
extern int D_00282164;
extern int D_00282168;
extern int D_0028216C;
extern int D_00282170;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810788;

void func_001FC280(void) {
    int *p;
    int v;
    int lo;
    int id;

    p = (int *)((char *)D_0024D650[D_00810700][D_00810701] + D_00810702 * 0x30);
    v = p[8];
    lo = v & 0xFFFF;
    id = (v & 0xFFFF0000) >> 16;
    if (D_00810700 == 0xB) {
        if (D_00810788 == 0xFF) {
            id = 0x44E;
        }
    }
    if (D_00282160 != id) {
        if (D_00282160 != -1) {
            func_0011A070(D_00282164);
        }
        D_00282160 = id;
        if (id != -1) {
            D_00282170 = 0x1000;
            D_0028216C = 0x1000;
            D_00282168 = 0x1000;
            D_00282164 = func_001FB9F0(D_00282160, D_00282170, D_00282168, D_0028216C);
        }
    }
    func_00119828(0, lo, lo);
    func_00119828(1, lo, lo);
}
