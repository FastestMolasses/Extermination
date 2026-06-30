// NEARMISS func_001FC3C0  (vram 0x001FC3C0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring (s0/s1/s2 assignment order) + one scheduling reorder of the func_0011A070 call. Body fully correct; not the clean-store nop, so 2.3.3 does not close it. Permuter/regalloc-permutation class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 86.9% (mwcc233) / 75.9% (991202). Body/structure fully recovered;
// sole residual is register-allocation COLORING (s0/s1/s2 assignment order)
// plus one scheduling reorder of the leftover-arg func_0011A070 call. Permuter
// territory, not the clean-store nop. arg0 (a0) is an unused hidden first param:
// the real pointer arg1 arrives in a1, arg2 in a2. func_0011A070 is called with
// only a0=idx; the extra args are leftover-register noise (declared () form).

extern int D_00281C30[];
extern int D_00281B70[];

extern void func_0011A070();
extern int func_001FBD50(int a, int b);
extern int func_001FBDB0(int a, short b);

void func_001FC3C0(int arg0, int *arg1, int arg2) {
    int idx;
    int v1;
    short ang;

    idx = *arg1;
    if (idx != -1) {
        v1 = D_00281C30[idx];
        if (v1 == -1) {
            *arg1 = -1;
            return;
        }
        if (arg2 != v1) {
            func_0011A070(idx, idx * 4, -1);
            D_00281B70[*arg1] = -1;
            *arg1 = -1;
            return;
        }
        ang = *(short *)0x70003B8A;
        if ((*(int *)0x70003B68 + ang) % 10 == 0) {
            *arg1 = func_001FBDB0(idx, ang);
            if (*arg1 == -1) {
                D_00281B70[idx] = -1;
            }
        }
    } else {
        if ((*(int *)0x70003B68 + *(short *)0x70003B8A) % 10 == 0) {
            *arg1 = func_001FBD50(arg2, 0);
            if (*arg1 != -1) {
                D_00281B70[*arg1] = arg2;
            }
        }
    }
}
