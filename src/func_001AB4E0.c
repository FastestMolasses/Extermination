// NEARMISS func_001AB4E0  (vram 0x001AB4E0, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Constant-materialization + register-coloring near-miss: target keeps the masked-0 (andi a2,zero,0x1FF) and 0x38 (daddiu a0,zero,0x38) in registers and shares the ~0x1FF mask reg; mwcc folds 0&0x1FF and uses immediate ori. Also s0/s1 save-slot ordering differs. Body/structure fully correct. Not th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001002E0(void *, int, int, int, void *, short);
extern unsigned char D_00810EA0;
extern unsigned short D_00810EB0;
extern unsigned char D_00810EC8;
extern unsigned short D_00810ED8;

void func_001AB4E0(void *arg0, int arg1) {
    short s = (short)(arg1 << 1);
    func_001002E0(&D_00810EA0, 0, 0x200, 0xE0, arg0, s);
    func_001002E0(&D_00810EC8, 0, 0x200, 0xE0, arg0, s);
    D_00810EB0 = (D_00810EB0 & ~0x1FF) | (0 & 0x1FF);
    D_00810ED8 = (D_00810ED8 & ~0x1FF) | 0x38;
}
