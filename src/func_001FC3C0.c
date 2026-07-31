// NEARMISS func_001FC3C0  (vram 0x001FC3C0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-tick service for one sound slot: keeps *arg1 (the live voice handle) in step
// with the requested sound id arg2 -- drops the voice when the table says it died or
// the id changed, and (re)starts it on the 10-tick scratchpad cadence at 0x70003B68.

extern int D_00281C30[];
extern int D_00281B70[];

extern void func_0011A070(int voice);
extern int func_001FBD50(int self, int id, int flags);
extern int func_001FBDB0(int self, int voice, int ang);

void func_001FC3C0(int arg0, int *arg1, int arg2) {
    int idx;
    int v1;
    int ang;
    int r;

    r = *arg1;
    idx = r;
    if (idx != -1) {
        v1 = D_00281C30[idx];
        if (v1 == -1) {
            *arg1 = -1;
            return;
        }
        if (arg2 != v1) {
            func_0011A070(idx);
            D_00281B70[*arg1] = -1;
            *arg1 = -1;
            return;
        }
        ang = *(short *)0x70003B8A;
        if ((*(int *)0x70003B68 + ang) % 10 == 0) {
            r = func_001FBDB0(arg0, idx, ang);
            *arg1 = r;
            if (r == -1) {
                D_00281B70[idx] = -1;
            }
        }
    } else {
        ang = *(short *)0x70003B8A;
        if ((*(int *)0x70003B68 + ang) % 10 == 0) {
            r = func_001FBD50(arg0, arg2, 0);
            *arg1 = r;
            if (r != -1) {
                D_00281B70[*arg1] = arg2;
            }
        }
    }
}
