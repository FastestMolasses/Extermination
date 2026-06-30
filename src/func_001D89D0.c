// NEARMISS func_001D89D0  (vram 0x001D89D0, 0x21C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body fully correct: 5-arg func_001D8340 (pointer as 5th arg) per matched sibling func_001D88B0, gp-rel D_00275670/D_00275688, non-gp D_00817BC0[] array form, (sub&0xFF)*4 index ordering. Residuals: (1) v0<->v1 register coloring on the `*(arg0+2)` byte reloads + commutative addu operand order on t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Per-mode dispatch on the global mode word at D_00275670+0x246C. Modes
// 1/3/4/5/6 tail into func_001D8C30(mode). The default mode publishes the
// active record pointer D_00275688 = &D_00817BC0, then builds the transform:
// the func_001D8130/func_001D8340 stage uses arg0+0xB0 when the sub-id byte at
// arg0+0x98 is 0xFF, otherwise the pointer at arg0+0x110+sub*4 (+0xC0); that
// pointer is also passed as func_001D8340's 5th argument. The 0x20 flag bit of
// arg0+2 selects a path inside those helpers. Then func_001D8690 runs with the
// 0x20 bit, and -- when bit 0x40 of arg0+2 is set -- 64*velocity from arg0+0x80..
// is accumulated into arg2+0x30.. with the last component clamped
// (max(arg0[0x8C]-1, 0)).
//
// NEARMISS: readable C, body byte-correct, 97.6% on mwcc 2.3.3 (991202 92.7%).
// Residuals are v0/v1 register coloring on the arg0+2 byte reloads and the same
// clamp bc1tl branch-likely + FP-coloring artifact as func_001D8690; not the
// clean-store nop. Permuter/register-coloring class.

extern void func_001D8130(int flag, int p);
extern void func_001D8340(int a, int b, int c, int flag, int p);
extern void func_001D8690(int a, int b, int c, int flag);
extern void func_001D8C30(int mode);
extern char *D_00275670;
extern void *D_00275688;
extern char D_00817BC0[];

void func_001D89D0(char *arg0, int arg1, char *arg2, int arg3) {
    int mode;
    unsigned char sub;
    int m;
    float c;

    mode = *(int *)(D_00275670 + 0x246C);
    if (mode == 1) {
        func_001D8C30(1);
        return;
    }
    if (mode == 3) {
        func_001D8C30(3);
        return;
    }
    if (mode == 4) {
        func_001D8C30(4);
        return;
    }
    if (mode == 5) {
        func_001D8C30(5);
        return;
    }
    if (mode == 6) {
        func_001D8C30(6);
        return;
    }

    D_00275688 = &D_00817BC0;
    sub = *(unsigned char *)(arg0 + 0x98);
    if (sub == 0xFF) {
        func_001D8130(*(unsigned char *)(arg0 + 2) & 0x20, (int)(arg0 + 0xB0));
        func_001D8340((int)arg0, arg1, (int)arg2, *(unsigned char *)(arg0 + 2) & 0x20, (int)(arg0 + 0xB0));
    } else {
        m = *(int *)((sub & 0xFF) * 4 + (int)arg0 + 0x110);
        func_001D8130(*(unsigned char *)(arg0 + 2) & 0x20, m + 0xC0);
        func_001D8340((int)arg0, arg1, (int)arg2, *(unsigned char *)(arg0 + 2) & 0x20, m + 0xC0);
    }
    func_001D8690(arg1, (int)arg2, arg3, *(unsigned char *)(arg0 + 2) & 0x20);
    if (*(unsigned char *)(arg0 + 2) & 0x40) {
        *(float *)(arg2 + 0x30) += 64.0f * *(float *)(arg0 + 0x80);
        *(float *)(arg2 + 0x34) += 64.0f * *(float *)(arg0 + 0x84);
        *(float *)(arg2 + 0x38) += 64.0f * *(float *)(arg0 + 0x88);
        c = *(float *)(arg0 + 0x8C) - 1.0f;
        if (c <= 0.0f) {
            c = 0.0f;
        }
        *(float *)(arg2 + 0x3C) += 64.0f * c;
    }
}
