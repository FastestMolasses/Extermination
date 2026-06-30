// NEARMISS func_001D1AE0  (vram 0x001D1AE0, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// List-scheduler / register-allocation artifact (permuter territory). Body and logic fully recovered (struct-table-base offset initializer). Sole residual is the mwcc list-scheduler interleaving the repeated gp-relative D_00275670 struct-pointer reloads against the 0x18/0x1C table-index computation...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (83.62% mwcc 2.3.3; 59.71% 991202) -- body/logic correct; sole
// residual is a list-scheduler interleave of the repeated D_00275670 struct
// reloads vs the 0x18/0x1C table-index computations (see wall note).
//
// Initializes a global subsystem struct (*D_00275670) for slot arg0: stores the
// slot index at +0x9C, then sets four char* table pointers into the big table
// D_0028F700 at per-slot strides --
//   +0x10 = &D_0028F700 + arg0*0x60800 + 0x8000
//   +0x14 = &D_0028F700 + arg0*0x95760 + 0xC9000
//   +0x18 = &D_0028F700 + arg0*0x70000 + 0x1F3EC0
//   +0x1C = &D_0028F700 + arg0*0x100000 + 0x2D3EC0
// (the 0x60800 stride is emitted as the shift-add chain ((arg0*3<<6)+arg0)<<11,
// the 0x70000 stride as (arg0*8-arg0)<<16) -- zeroes the four ints at +0x50/
// +0x54/+0x58/+0x5C, then runs the init sequence:
//   func_001CBA40(self, +0x18 ptr); func_001D1F20(1); func_001D2040(1, 0);
//   func_001D1FF0(1, 1); func_001CB8A0(&D_007635C0, 0, self, self+4);
//   func_001D2DE0(0, 0).
// -sdatathreshold 4 keeps the char* D_00275670 gp-relative; the far table bases
// D_0028F700 / D_007635C0 are over-declared as arrays for absolute addressing.
extern void func_001CB8A0(void *a, int b, char *c, char *d);
extern void func_001CBA40(char *a, char *b);
extern void func_001D1F20(int a);
extern void func_001D1FF0(int a, int b);
extern void func_001D2040(int a, int b);
extern void func_001D2DE0(int a, int b);
extern char *D_00275670;
extern char D_0028F700[0x100];
extern char D_007635C0[0x100];

void func_001D1AE0(int arg0) {
    char *p;

    *(int *)(D_00275670 + 0x9C) = arg0;
    *(char **)(D_00275670 + 0x10) = D_0028F700 + arg0 * 0x60800 + 8 + 0x7FF8;
    *(char **)(D_00275670 + 0x14) = D_0028F700 + arg0 * 0x95760 + 0xC9000;
    p = D_0028F700 + arg0 * 0x70000 + 0x1F3EC0;
    *(char **)(D_00275670 + 0x18) = p;
    *(char **)(D_00275670 + 0x1C) = D_0028F700 + arg0 * 0x100000 + 0x2D3EC0;
    *(int *)(D_00275670 + 0x50) = 0;
    *(int *)(D_00275670 + 0x5C) = 0;
    *(int *)(D_00275670 + 0x58) = 0;
    *(int *)(D_00275670 + 0x54) = 0;
    func_001CBA40(D_00275670, p);
    func_001D1F20(1);
    func_001D2040(1, 0);
    func_001D1FF0(1, 1);
    func_001CB8A0(D_007635C0, 0, D_00275670, D_00275670 + 4);
    func_001D2DE0(0, 0);
}
